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
#ifndef SEQUOIA_DYN_PROG_SOLVER_H
#define	SEQUOIA_DYN_PROG_SOLVER_H

#include "structures/labeled_graph.h"
#include "structures/treedecomposition.h"

namespace sequoia {

class DynProgSolver {
public:
    DynProgSolver() : _graph(NULL), _treedecomposition(NULL) { }
    virtual void load_graph(const char *filename);
    virtual void load_treedecomposition(const char *filename);
    virtual void solve();
    
    const LabeledGraph* graph() const { return _graph; }
    void graph(const LabeledGraph* graph) { this->_graph = graph; }
    TreeDecomposition* treedecomposition() { return _treedecomposition; }
    const TreeDecomposition* treedecomposition() const {
        return _treedecomposition;
    }
    void treedecomposition(TreeDecomposition* treedecomposition) {
        this->_treedecomposition = treedecomposition;
    }
    
    virtual void check_treedecomposition();
    virtual void work_on(const TreeDecomposition::vertex_descriptor& t);
    virtual void do_leaf(const TreeDecomposition::vertex_descriptor& t) = 0;
    virtual void do_root(const TreeDecomposition::vertex_descriptor& t) = 0;
    virtual void do_introduce(const TreeDecomposition::vertex_descriptor& child,
                              const TreeDecomposition::vertex_descriptor& t) = 0;
    virtual void do_forget(const TreeDecomposition::vertex_descriptor& child,
                           const TreeDecomposition::vertex_descriptor& t) = 0;
    virtual void do_join(const TreeDecomposition::vertex_descriptor& left,
                         const TreeDecomposition::vertex_descriptor& right,
                         const TreeDecomposition::vertex_descriptor& t) = 0;

protected:
    virtual void generate_treedecomposition();
    virtual void pre_solve();
    virtual void post_solve() = 0;
private:
    const LabeledGraph* _graph;
    TreeDecomposition* _treedecomposition;
};

} // namespace

#endif	/* SEQUOIA_DYN_PROG_SOLVER_H */

