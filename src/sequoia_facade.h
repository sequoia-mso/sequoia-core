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
#ifndef SEQUOIA_FACADE_H
#define	SEQUOIA_FACADE_H

#include "exceptions.h"

#include <vector>
#include <set>

namespace sequoia {

namespace internal {
struct SequoiaSolverFacadeData;
}

class SequoiaSolverFacade {
public:
    typedef int vertex_t;
    /**
     * Create a new facade object with an empty graph.
     */
    SequoiaSolverFacade();
    virtual ~SequoiaSolverFacade();

    /**
     * Add a vertex to the facade object
     * @param f facade object
     * @return the newly created error.
     */
    vertex_t add_vertex();

    /**
     * Add an edge between the two specified vertices u, v.  This is a directed
     * graph, so make sure you add back edges for undirected graphs.
     * @param u vertex
     * @param v vertex
     */
    void add_edge(vertex_t u, vertex_t v) throw (sequoia_exception);

    /**
     * Solve the problem specified by the formula on the graph.
     * @param formula	    the MSO formula
     * @param evaluation    the evaluation to use.  Refer to the documentation
     *			    for possible values.
     * @return		    true iff a solution was found
     */
    bool solve(const std::string &formula,
               const std::string &evaluation)
    throw (sequoia_exception);

    /**
     * Return the solution if there is one.
     * Prerequisite:  solve() returned true
     */
    std::vector<std::set<vertex_t> > solution()
    throw (sequoia_exception);

private:
    internal::SequoiaSolverFacadeData *_data;
};

} // namespace

#endif	/* SEQUOIA_FACADE_H */

