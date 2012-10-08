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
#include "exceptions.h"
#include "dyn_prog_solver.h"
#include "mindegree_heuristic.h"
#include "structures/graph_factory.h"
#include "structures/treedecomposition_check.h"

#include <list>
#include <vector>

#ifdef HAVE_TBB
#include <tbb/task.h>
#endif

namespace sequoia {

void DynProgSolver::load_graph(const char *filename) {
    if (filename == NULL)
	throw sequoia_usage_error("No graph file specified.");

    LabeledGraph *g = new LabeledGraph();
    GraphFactory<LabeledGraph>::load_graph(*g, filename);
    _graph = g;
}

void DynProgSolver::load_treedecomposition(const char *filename) {
    std::cout << "Loading tree decomposition from file " << filename << std::endl;
    if (filename == NULL)
	throw sequoia_usage_error("Invalid tree decomposition file given (NULL)");
    TreeDecomposition* tdc = new TreeDecomposition();
    GraphFactory<TreeDecomposition>::load_graph(*tdc, filename);
    _treedecomposition = tdc;
}

void DynProgSolver::generate_treedecomposition() {
    std::cout << "Generating tree decomposition..." << std::endl;
    MinDegreeHeuristic<sequoia::LabeledGraph> min_degree(*_graph);
    min_degree.compute();
    _treedecomposition = min_degree.get();
}

void DynProgSolver::check_treedecomposition() {
    std::cout << "Checking tree decomposition..." << std::endl;
    typedef TreeDecompositionCheck<sequoia::LabeledGraph> Checker;
    Checker check(*_graph, *_treedecomposition);
    if (_treedecomposition->num_vertices() > 5000) {
        std::cout << "Not checking whether tree decomposition is valid, graph is too large" << std::endl;
        return;
    } else {
        if (!check.valid())
	    throw sequoia_internal_error("Tree decomposition is not a valid tree decomposition.");
    }
    if (!check.is_nice())
	throw sequoia_internal_error("Tree decomposition is not a valid nice tree decomposition.");
}

void DynProgSolver::pre_solve() {
    if (graph() == NULL)
	throw sequoia_usage_error("No graph loaded.");
    if (treedecomposition() == NULL)
        generate_treedecomposition();

    std::cout << "Making tree decomposition nice..." << std::endl;
    _treedecomposition->make_nice();
    check_treedecomposition();

    int width = _treedecomposition->width();
    std::cout << "Tree decomposition has width: " << width - 1 << " (" << width << " cops)" << std::endl;
}

#ifndef HAVE_TBB
void DynProgSolver::solve() {
    DPRINTLN("DynProgSolver::solve()");
    this->pre_solve();

    typedef TreeDecomposition::vertex_descriptor Vertex;
    typedef TreeDecomposition::edge_descriptor Edge;
    typedef TreeDecomposition::out_edge_iterator OutEdgeIterator;

    const TreeDecomposition* tdc = _treedecomposition;

    int counter = 0;
    std::list<Vertex> stack;
    std::vector<bool> visited(tdc->num_vertices());
    stack.push_front(tdc->root());
    do {
        Vertex v = stack.front();

        if (visited[v] == false) {
            OutEdgeIterator i, iend;
            for (boost::tie(i, iend) = tdc->out_edges(v); i != iend; ++i) {
                stack.push_front(tdc->target(*i));
            }
            visited[v] = true;
        } else {
            stack.pop_front();
            work_on(v);
        }
    } while(stack.size() > 0);
    do_root(tdc->root());

    this->post_solve();
}

void
DynProgSolver::work_on(const TreeDecomposition::vertex_descriptor& t) {
    const TreeDecomposition* tdc = _treedecomposition;
    if (tdc->out_degree(t) == 0) {
        do_leaf(t);
        return;
    }
    if (tdc->out_degree(t) == 1) {
        TreeDecomposition::out_edge_iterator o, oend;
        boost::tie(o, oend) = tdc->out_edges(t);
        TreeDecomposition::vertex_descriptor child = tdc->target(*o);
        if (tdc->bag(child)->width() < tdc->bag(t)->width())
            do_introduce(child, t);
        else
            do_forget(child, t);
        return;
    }
    if (tdc->out_degree(t) == 2) {
        TreeDecomposition::out_edge_iterator o, oend;
        boost::tie(o, oend) = tdc->out_edges(t);
        TreeDecomposition::vertex_descriptor left = tdc->target(*o++);
        TreeDecomposition::vertex_descriptor right = tdc->target(*o++);
        do_join(left, right, t);
        return;
    }
    assert(false);
}

#else

class DynProgContinuationTask : public tbb::task {
    typedef TreeDecomposition::vertex_descriptor Vertex;
public:
    DynProgContinuationTask(DynProgSolver *s, const Vertex v)
    : solver(s), vertex(v) { }

    tbb::task* execute() { 
        const TreeDecomposition* tdc = solver->treedecomposition();
        if (tdc->out_degree(vertex) == 1) {
            TreeDecomposition::out_edge_iterator o, oend;
            boost::tie(o, oend) = tdc->out_edges(vertex);
            TreeDecomposition::vertex_descriptor child = tdc->target(*o);
            if (tdc->bag(child)->width() < tdc->bag(vertex)->width())
                solver->do_introduce(child, vertex);
            else
                solver->do_forget(child, vertex);
            return NULL;
        }
        if (tdc->out_degree(vertex) == 2) {
            TreeDecomposition::out_edge_iterator o, oend;
            boost::tie(o, oend) = tdc->out_edges(vertex);
            TreeDecomposition::vertex_descriptor left = tdc->target(*o++);
            TreeDecomposition::vertex_descriptor right = tdc->target(*o++);
            solver->do_join(left, right, vertex);
            return NULL;
        }
	assert(false);
	return NULL;
    }
private:
    DynProgSolver* solver;
    const Vertex vertex;
};

class DynProgScheduleTask : public tbb::task {
public:
    typedef TreeDecomposition::vertex_descriptor Vertex;

    DynProgScheduleTask(DynProgSolver *s, const Vertex v)
    : solver(s), vertex(v) { } 

    tbb::task* execute() {
        const TreeDecomposition* tdc = solver->treedecomposition();

        if (tdc->out_degree(vertex) == 0) {
            solver->do_leaf(vertex);
            return NULL;
        }

	// schedule continuation task for this vertex
	DynProgContinuationTask& c =
		*new(tbb::task::allocate_continuation()) DynProgContinuationTask(solver, vertex);

	// schedule dynprog tasks for all children of this node
	TreeDecomposition::out_edge_iterator o, oend;
	boost::tie(o, oend) = tdc->out_edges(vertex);
	Vertex first_child = tdc->target(*o++);
	int seen = 1;
	for (; o != oend; o++) {
            DynProgScheduleTask& lt = *new(c.allocate_child()) DynProgScheduleTask(solver, tdc->target(*o));
	    spawn(lt);
	    seen++;
	}
	c.set_ref_count(seen);
	tbb::task::recycle_as_child_of(c); 
	vertex = first_child;
	return this;
    }
private:
    DynProgSolver* solver;
    Vertex vertex;
};


void DynProgSolver::solve() {
    DPRINTLN("DynProgSolver::solve()");
    this->pre_solve();
    typedef TreeDecomposition::vertex_descriptor Vertex;
    typedef TreeDecomposition::edge_descriptor Edge;
    typedef TreeDecomposition::out_edge_iterator OutEdgeIterator;
    const TreeDecomposition* tdc = _treedecomposition;
    DynProgScheduleTask& rt = *new( tbb::task::allocate_root() ) DynProgScheduleTask(this, tdc->root());
    tbb::task::spawn_root_and_wait(rt);
    do_root(tdc->root());
    this->post_solve();
}


void
DynProgSolver::work_on(const TreeDecomposition::vertex_descriptor& t) {
    assert(false);
}

#endif

} // namespace
