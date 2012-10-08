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
#ifndef SEQUOIA_APPLY_ROOT_H
#define SEQUOIA_APPLY_ROOT_H

#include "sequoia_table.h"

namespace sequoia {

template <typename Solver>
class ApplyRoot {
public:
    ApplyRoot<Solver>(Solver *solver,
		      const TreeDecomposition::vertex_descriptor& node)
    : _solver(solver), _node(node), _has_solution(false), _solution(NULL) { }
    void init();
    void operator()();
    bool has_solution() const { return _has_solution; }
    const void* solution() const { return _solution; }
protected:
    Solver *_solver;
    const TreeDecomposition::vertex_descriptor &_node;
    const Bag* _node_bag;
    bool _has_solution;
    const void *_solution;
private:
    ApplyRoot<Solver>(const ApplyRoot<Solver> &); // forbid
};

} // namespace

#include "apply_root.hpp"

#endif //  SEQUOIA_APPLY_ROOT_H