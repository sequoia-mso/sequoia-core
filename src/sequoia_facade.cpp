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
#include "sequoia.h"
#include "sequoia_facade.h"
#include "sequoia_solver.h"
#include "exceptions.h"
#include "structures/graph_structure_factory.h"

namespace sequoia {

namespace internal {
struct SequoiaSolverFacadeData {
    LabeledGraph graph;
    SequoiaSolver solver;
};
}

SequoiaSolverFacade::SequoiaSolverFacade()
: _data(new internal::SequoiaSolverFacadeData) { 
}

SequoiaSolverFacade::~SequoiaSolverFacade() {
    delete _data;
}

SequoiaSolverFacade::vertex_t
SequoiaSolverFacade::add_vertex() {
    return _data->graph.add_vertex();
}

void
SequoiaSolverFacade::add_edge(SequoiaSolverFacade::vertex_t u,
			      SequoiaSolverFacade::vertex_t v)
throw (sequoia_exception) {
    if (_data->graph.num_vertices() <= u) {
	std::stringstream s;
	s << "Invalid vertex u: " << u;
	throw sequoia_usage_error(s.str());
    }
    if (_data->graph.num_vertices() <= v) {
	std::stringstream s;
	s << "Invalid vertex v: " << v;
	throw sequoia_usage_error(s.str());
    }
    _data->graph.add_edge(u,v);
}

bool
SequoiaSolverFacade::solve(const std::string &formula,
			   const std::string &evaluation)
throw (sequoia_exception) {
    SequoiaSolver &solver = _data->solver;
    solver.graph(GraphStructureFactory::make(new LabeledGraph(_data->graph)));
    solver.formula(formula);
    solver.load_evaluation(evaluation);
    solver.solve();
    return solver.has_solution();
}

std::vector<std::set<SequoiaSolverFacade::vertex_t> >
SequoiaSolverFacade::solution() throw (sequoia_exception) {
    SequoiaSolver &solver = _data->solver;
    const void *sol = solver.solution();
    const Evaluation* eval = solver.evaluation();
    
    std::vector<std::set<vertex_t> > res;

    if (!eval->returns_vertex_sets())
	return res;

    std::vector<std::set<int> > sets = eval->convert_to_vertex_sets(sol);
    const std::vector<const UnarySymbol*> &free_vars = solver.free_unary_symbols();
    for (int i = 0; i < free_vars.size(); i++) {
	const UnarySymbol *sym = free_vars[i];
	res.push_back(std::set<vertex_t>());
	std::set<int>::const_iterator it;
	for (it = sets[i].begin(); it != sets[i].end(); it++)
	    res[i].insert(*it);
    }
    return res;
}

} // namespace

extern "C" {

sequoia_t sequoia_create() {
    return new sequoia::SequoiaSolverFacade();
}

void sequoia_destroy(sequoia_t f) {
    sequoia::SequoiaSolverFacade *facade = (sequoia::SequoiaSolverFacade*) f;
    delete facade;
}

int sequoia_add_vertex(sequoia_t f) {
    sequoia::SequoiaSolverFacade *facade = (sequoia::SequoiaSolverFacade*) f;
    return facade->add_vertex();
}

int sequoia_add_edge(sequoia_t f, int u, int v) {
    sequoia::SequoiaSolverFacade *facade = (sequoia::SequoiaSolverFacade*) f;
    try {
	facade->add_edge(u, v);
    } catch (const sequoia::sequoia_exception &e) {
	std::cerr << "Sequoia error: " << e.what() << std::endl;
	return 1;
    }
    return 0;
}

int sequoia_solve(sequoia_t f, const char *formula,
			 const char *evaluation) {
    sequoia::SequoiaSolverFacade *facade = (sequoia::SequoiaSolverFacade*) f;
    bool result = false;
    try {
	result = facade->solve(std::string(formula), std::string(evaluation));
    } catch (const sequoia::sequoia_exception &e) {
	std::cerr << "Sequoia error: " << e.what() << std::endl;
	return -1;
    }
    if (result) return 1; else return 0;
}

int * sequoia_solution(sequoia_t f) {
    sequoia::SequoiaSolverFacade *facade = (sequoia::SequoiaSolverFacade*) f;
    typedef std::vector<std::set<sequoia::SequoiaSolverFacade::vertex_t> > res_t;
    res_t res = facade->solution();

    // compute space needed
    size_t count = 1 + res.size();
    res_t::const_iterator it;
    for (it = res.begin(); it != res.end(); it++)
	count += it->size();

    // now convert the sets into a compacted, C-compatible int*
    int *ptr = (int*) ::calloc(count, sizeof(int));
    int *p = ptr;
    *p++ = res.size();
    for (it = res.begin(); it != res.end(); it++) {
	*p++ = it->size();
	std::set<int>::const_iterator sit;
	for (sit = it->begin(); sit != it->end(); sit++)
	    *p++ = *sit;
    }
    return ptr;
}

} // extern "C"




