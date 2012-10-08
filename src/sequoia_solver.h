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
#ifndef SEQUOIA_SEQUOIA_SOLVER_H
#define SEQUOIA_SEQUOIA_SOLVER_H

#include "structures/graph.h"
#include "structures/graph_structure.h"

#include "dyn_prog_solver.h"
#include "sequoia_eval.h"
#include "sequoia_table.h"

#include <boost/thread.hpp>
#if HAVE_BOOST_TIMER
#include <boost/timer/timer.hpp>
#endif

#ifdef HAVE_TBB
#include <tbb/mutex.h>
#include <tbb/atomic.h>
#endif

namespace sequoia {

class SequoiaSolver : public DynProgSolver {
public:
    SequoiaSolver()
    : _graph(NULL), _formula(NULL), _evaluation(NULL),
      _create_incidence_graph(false), _solution(NULL) {
        // TBB requires late init
        _nodes_started = _nodes_printed = _games_completed = 0UL;
    }
    virtual ~SequoiaSolver();

    /* public interface */
    virtual void load_graph(const char *filename);
    void graph(const GraphStructure *graph);
    void create_incidence_graph(bool flag) { this->_create_incidence_graph = flag; }
    /* virtual void load_treedecomposition(const char *filename); // in DynProgSolver */
    virtual void load_formula(const char *filename);
    virtual void formula(const std::string &formula);
    virtual void load_evaluation(const std::string &name);

    void cache_size(size_t cache_size);

    /* virtual void solve(); // in DynProgSolver */

    bool has_solution() const { return _has_solution; }
    const void* solution() const { return _solution; }

    const GraphStructure *
    solution_graph() const;

    std::vector<const UnarySymbol*> const& free_unary_symbols() const {
	return _free_unary_symbols;
    }


    /* internal interface for algorithms */

    const Formula* formula() const { return _formula; }
    void formula(const Formula* formula) { this->_formula = formula; }
    SequoiaInternalEvaluation* evaluation() const { return _evaluation; }
    const GraphStructure* graph() const { return _graph; }

    const Assignment_f * base_alpha() const { return _base_alpha; }
    unsigned int n_terminals() { return _terminal_symbols.size(); }
    const ConstantSymbol* terminal_symbol(int i) const {
	return _terminal_symbols[i];
    }
    unsigned int n_free_unary_symbols() const { return _free_unary_symbols.size(); }
    const UnarySymbol* free_unary_symbol(int i) const {
	return _free_unary_symbols[i];
    }
    unsigned int signature_depth() const { return _vocabulary->size() - 1; }
    const Vocabulary* vocabulary() const { return _vocabulary; }
    SequoiaTable *table(const TreeDecomposition::vertex_descriptor& t) {
	return _tables[t];
    }
    void alpha(const TreeDecomposition::vertex_descriptor& t,
	       const Assignment_f* alpha) {
	this->_alphas[t] = alpha->clone();
    }
    const Assignment_f * alpha(const TreeDecomposition::vertex_descriptor& t) const {
	return this->_alphas[t];
    }

    void log_node_started(const TreeDecomposition::vertex_descriptor& t);
    void log_games_completed(size_t num);
    void log_node_completed(const TreeDecomposition::vertex_descriptor& t);
    void log_periodic();
    void log_status();

protected:
    virtual void pre_solve();
    virtual void post_solve();

    virtual void setup_graph();
    virtual void load_formula(const char *source,
                              Formula* (*parser)(const Vocabulary* vocabulary,
                                                 const char* string));

    virtual void setup_formula_vocabulary();
    virtual void finalize_formula_vocabulary();

    void do_leaf(const TreeDecomposition::vertex_descriptor& t);
    void do_root(const TreeDecomposition::vertex_descriptor& t);
    void do_introduce(const TreeDecomposition::vertex_descriptor& child,
                      const TreeDecomposition::vertex_descriptor& t);
    void do_forget(const TreeDecomposition::vertex_descriptor& child,
                   const TreeDecomposition::vertex_descriptor& t);
    void do_join(const TreeDecomposition::vertex_descriptor& left,
                 const TreeDecomposition::vertex_descriptor& right,
                 const TreeDecomposition::vertex_descriptor& t);

private:
    void cleanup(const TreeDecomposition::vertex_descriptor &t);

    const GraphStructure *_orig_graph;
    bool _create_incidence_graph;
    const GraphStructure *_graph;
    const Formula* _formula;
    SequoiaInternalEvaluation *_evaluation;

    Vocabulary* _vocabulary;  // vocabulary annotated with new symbols
    std::vector<const ConstantSymbol*> _terminal_symbols;
    std::vector<const UnarySymbol*> _free_unary_symbols;
    const Assignment_f *_base_alpha;

    std::vector<SequoiaTable*> _tables;
    std::vector<const Assignment_f*> _alphas;

    const void* _solution;
    bool _has_solution;

#ifdef HAVE_TBB
    tbb::atomic<size_t> _nodes_started;
    tbb::atomic<size_t> _nodes_printed;
    tbb::atomic<size_t> _games_completed;
#else
    size_t _nodes_started;
    size_t _nodes_printed;
    size_t _games_completed;
#endif
    void print_usage();
    class PeriodicJob {
    public:
        PeriodicJob(SequoiaSolver* s) : _solver(s), _stop(false) { }
        void operator()() {
            while (true) {
                boost::this_thread::sleep(boost::posix_time::seconds(5));
                if (_stop) break;
                _solver->log_periodic();
            }
        }
        void stop() { _stop = true; }
    private:
        SequoiaSolver *_solver;
        bool _stop;
    };
    PeriodicJob *_periodic;
    bool _skip_periodic;
    boost::mutex _log_mutex; 
#if HAVE_BOOST_TIMER
    boost::timer::cpu_timer _cpu_timer;
#endif
};

} // namespace

#endif // SEQUOIA_SEQUOIA_SOLVER_H
