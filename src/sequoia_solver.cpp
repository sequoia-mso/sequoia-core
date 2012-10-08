/*
 * This file is part of the Sequoia MSO Solver.
 * 
 * Copyright 2012 Alexander Langer, Theoretical Computer Science,
 *                                  RWTH Aachen University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @author Alexander Langer
 */
#include "mindegree_heuristic.h"
#include "moves_pool.h"
#include "parseformula.h"
#include "sequoia_solver.h"
#include "sequoia_eval_factory.h"
#include "structures/graph_structure_factory.h"
#include "structures/treedecomposition_check.h"
#include "apply_leaf.h"
#include "apply_introduce.h"
#include "apply_forget.h"
#include "apply_join.h"
#include "apply_root.h"
#include "cache_control.h"
#include "exceptions.h"
#include "incidence_graph.h"

#include <algorithm>

#include <boost/scoped_ptr.hpp>

#if !HAVE_BOOST_TIMER
#include <sys/time.h>
#include <sys/resource.h>
#endif

namespace sequoia {

SequoiaSolver::~SequoiaSolver() {
    for (unsigned int i = 0; i < _tables.size(); i++) {
	if (_tables[i] != NULL) 
	    delete _tables[i];
	if (_alphas[i] != NULL) 
	    delete _alphas[i];
    }
    delete _evaluation;
    if (_orig_graph != _graph)
        delete _orig_graph;
    delete _graph;
}

void SequoiaSolver::cache_size(size_t size) { 
    cache_resize(size);
}
        
void SequoiaSolver::pre_solve() {
    DynProgSolver::pre_solve();
    _has_solution = false;
    assert(_evaluation != NULL);
    _tables.resize(treedecomposition()->num_vertices());
    _alphas.resize(treedecomposition()->num_vertices());
    for (unsigned int i = 0; i < treedecomposition()->num_vertices(); i++) {
        _tables[i] = new SequoiaTable(treedecomposition(), i, _evaluation);
	_alphas[i] = NULL;
    }

    _periodic = new PeriodicJob(this);
    boost::thread thread(*_periodic);
    log_status();
}

void SequoiaSolver::load_graph(const char* filename) {
    try {
	_orig_graph = GraphStructureFactory::load(filename);
    } catch (const std::exception &e) {
	throw sequoia_file_error(
	    std::string("Cannot load graph: ") + std::string(e.what()));
    }
    std::cout << "Loaded Graph of order " << _orig_graph->graph()->num_vertices() << " with Vocabulary: "
            << _orig_graph->vocabulary()->toString() << std::endl;
}

void SequoiaSolver::graph(const GraphStructure *graph) {
    _orig_graph = graph;
}

void SequoiaSolver::setup_graph() {
    if (_create_incidence_graph) {
        std::cout << "Creating incidence graph..." << std::endl;
        LabeledGraph *incg = new LabeledGraph();
        sequoia::create_incidence_graph(*_orig_graph->graph(), *incg, treedecomposition());
	_graph = GraphStructureFactory::make(incg);
    }  else {
        _graph = _orig_graph;
    }
    DynProgSolver::graph(_graph->graph());
}

void SequoiaSolver::setup_formula_vocabulary() {
    if (treedecomposition() == NULL)
        generate_treedecomposition();

    boost::scoped_ptr<const Assignment_f> alpha(AssignmentFlyFactory::make(new EmptyAssignment()));
    for (unsigned int i = 0; i < _graph->num_labels(); i++) {
        const UnarySymbol* s = _graph->label(i);
        std::cout << "Found bound variable '"
                << s->identifier() << "' at depth "
                    << s->nesting_depth()
                    << std::endl;
        const SetMove *smove = moves_pool.pool(new SetMove(s));
        alpha.reset(AssignmentFlyFactory::make(new SetAssignment(alpha.get(), s, smove)));
    }
    _base_alpha = alpha->clone();

    int width = treedecomposition()->width();
    std::cout << "Adding constant symbols t0 ... t"
            << (width-1) << " to vocabulary..." << std::endl;
    _vocabulary = new Vocabulary(_graph->vocabulary());

    for (int i = 0; i < width; ++i) {
        std::stringstream s;
        s << "t" << i;
        const ConstantSymbol* tsym = _vocabulary->add_constant_symbol(s.str(), false);
        _terminal_symbols.push_back(tsym);
    }
    std::cout << "Annotated vocabulary: " << _vocabulary->toString() << std::endl;
}

void SequoiaSolver::load_formula(const char *source,
				 Formula* (*parser)(const Vocabulary* vocabulary,
						    const char* string)) {
    this->setup_graph();
    this->setup_formula_vocabulary();

    _formula = parser(_vocabulary, source);
    if (_formula == NULL)
        throw sequoia_formula_error("Cannot parse formula.");

    const Vocabulary* fvoc = _formula->vocabulary();
    std::cout << "Formula is: " << _formula->toString() << std::endl;
    std::cout << "Vocabulary: " << fvoc->toString() << std::endl;
    if (fvoc->number_of_constant_symbols() !=
            _vocabulary->number_of_constant_symbols())
        throw sequoia_formula_error("Formula has free constant symbols.");
    if (fvoc->size() >= BITSET_SIZE) {
        std::stringstream s;
        s << "Too many symbols. Cannot handle more than " << BITSET_SIZE << ".";
        throw sequoia_formula_error(s.str());
    }
    this->finalize_formula_vocabulary();
}

void SequoiaSolver::formula(const std::string &formula) {
    std::cout << "Parsing formula: " << formula << std::endl;
    this->load_formula(formula.c_str(), parse_string);
}

void SequoiaSolver::load_formula(const char* filename) {
    std::cout << "Loading formula from file " << filename << std::endl;
    this->load_formula(filename, parse_file);
}

void SequoiaSolver::finalize_formula_vocabulary() {
    const Vocabulary* fvoc = _formula->vocabulary();
    for (unsigned int i = _vocabulary->number_of_unary_symbols();
            i < fvoc->number_of_unary_symbols(); ++i) {
        const UnarySymbol* s = fvoc->unary_symbol(i);
        if (s->nesting_depth() < _vocabulary->size())
            continue;
        std::cout << "Found free variable '" << s->identifier()
                << "' at depth " << s->nesting_depth() << std::endl;
        std::string ident(s->identifier());
        if (ident == "Vertex" || ident == "Edge")
            throw sequoia_incident_symbol_found(ident);
        _free_unary_symbols.push_back(s);
    }
}

void SequoiaSolver::load_evaluation(const std::string &name) {
    _evaluation = sequoia_eval_make(name,
                                    _terminal_symbols,
                                    _free_unary_symbols);
}

void
SequoiaSolver::do_leaf(const TreeDecomposition::vertex_descriptor& t) {
    DPRINTLN("#####################################################");
    DPRINTLN("############# LEAF ##################################");
    DPRINTLN("#####################################################");
    log_node_started(t);
    ApplyLeaf<SequoiaSolver> apply(this, t);
    apply.init();
    apply();
    log_node_completed(t);
}

void
SequoiaSolver::do_introduce(const TreeDecomposition::vertex_descriptor& child,
			    const TreeDecomposition::vertex_descriptor& t) {
    DPRINTLN("#####################################################");
    DPRINTLN("############# INTRODUCE #############################");
    DPRINTLN("#####################################################");
    log_node_started(t);
    ApplyIntroduce<SequoiaSolver> apply(this, t, child);
    apply.init();
    apply();
    cleanup(child);
    log_node_completed(t);
}

void
SequoiaSolver::do_forget(const TreeDecomposition::vertex_descriptor& child,
                      const TreeDecomposition::vertex_descriptor& t) {
    DPRINTLN("#####################################################");
    DPRINTLN("############# FORGET ################################");
    DPRINTLN("#####################################################");
    log_node_started(t);
    ApplyForget<SequoiaSolver> apply(this, t, child);
    apply.init();
    apply();
    cleanup(child);
    log_node_completed(t);
}

void
SequoiaSolver::do_join(const TreeDecomposition::vertex_descriptor& left,
                    const TreeDecomposition::vertex_descriptor& right,
                    const TreeDecomposition::vertex_descriptor& t) {
    DPRINTLN("#####################################################");
    DPRINTLN("############# JOIN ##################################");
    DPRINTLN("#####################################################");
    log_node_started(t);
    ApplyJoin<SequoiaSolver> apply(this, t, left, right);
    apply.init();
    apply();
    cleanup(left);
    cleanup(right);
    log_node_completed(t);
}

void
SequoiaSolver::do_root(const TreeDecomposition::vertex_descriptor& t) {
    DPRINTLN("#####################################################");
    DPRINTLN("############# ROOT ##################################");
    DPRINTLN("#####################################################");
    log_node_started(t);
    ApplyRoot<SequoiaSolver> apply(this, t);
    apply.init();
    apply();
    _has_solution = apply.has_solution();
    _solution = apply.solution();
    cleanup(t);
    log_node_completed(t);
    _periodic->stop();
}

void SequoiaSolver::post_solve() {
    std::cout << std::endl;
}

void SequoiaSolver::cleanup(const TreeDecomposition::vertex_descriptor& t) {
    SequoiaTable *tab = _tables[t];
    SequoiaTable::const_iterator it;
    delete tab;
    _tables[t] = NULL;
    if (_alphas[t] != NULL) {
	delete _alphas[t];
	_alphas[t] = NULL;
    }
}

const GraphStructure *
SequoiaSolver::solution_graph() const {
    LabeledGraph *result = new LabeledGraph(*_graph->graph());
    if (!has_solution()) return NULL;

    // if the evaluation can return the solution as a vector of vertex sets,
    // annotate the graph with the solution
    if (evaluation()->returns_vertex_sets()) {
	std::vector<std::set<int> > sets = _evaluation->convert_to_vertex_sets(_solution);
	for (int i = 0; i < _free_unary_symbols.size(); i++) {
	    const UnarySymbol *sym = _free_unary_symbols[i];
	    int lab_id = result->create_label(std::string(sym->identifier()));
	    std::set<int>::const_iterator it;
	    for (it = sets[i].begin(); it != sets[i].end(); it++)
		result->vertex_add_label(*it, lab_id);
	}
    }

    // possibly revert the incidence graph creation used earlier
    if (!_create_incidence_graph)
	return GraphStructureFactory::make(result);
    LabeledGraph *adj_result = new LabeledGraph();
    create_adjacency_graph(*result, *adj_result);
    delete result;
    return GraphStructureFactory::make(adj_result);
}

void
SequoiaSolver::log_node_started(const TreeDecomposition::vertex_descriptor& t) {
    const TreeDecomposition* tdc = treedecomposition();
    boost::lock_guard<boost::mutex> lock(_log_mutex); 
    std::cout << " " << tdc->bag(t)->width() << std::flush;
    if (tdc->out_degree(t) == 2)
        std::cout << "J" << std::flush;
    _nodes_started++;

    if (++_nodes_printed >= 10) {
        _nodes_printed = 0;
        _skip_periodic = true;
	log_status();
    }
}

void
SequoiaSolver::log_node_completed(const TreeDecomposition::vertex_descriptor& t) {
}

void
SequoiaSolver::log_games_completed(size_t num) {
    _games_completed += num;
}

void
SequoiaSolver::print_usage() {
#if HAVE_BOOST_TIMER
#ifdef HAVE_TBB
    std::cout << _cpu_timer.format(1, "%ws [%us+%ss=%p%]");
#else
    std::cout << _cpu_timer.format(1, "%ws [%us=%p%]");
#endif // HAVE_TBB
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    std::cout << "CPU: ";
    std::cout << usage.ru_utime.tv_sec << "s user ";
#ifdef HAVE_TBB
    std::cout << usage.ru_stime.tv_sec << "s system ";
#endif // HAVE_TBB
#endif // HAVE_BOOST_TIMER
}

void
SequoiaSolver::log_status() {
    const TreeDecomposition* tdc = treedecomposition();
    std::cout << std::endl;
    std::cout << "Node " << _nodes_started
        << "/" << tdc->num_vertices()
	<< " game " << _games_completed << " ";
    print_usage();
    std::cout << std::flush;
}

void
SequoiaSolver::log_periodic() {
    if (_skip_periodic) {
        _skip_periodic = false;
        return;
    }
    boost::lock_guard<boost::mutex> lock(_log_mutex); 
    _nodes_printed = 0;
    log_status();
}

} // namespace
