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
#ifndef SEQUOIA_APPLY_GAME_JOIN_H
#define SEQUOIA_APPLY_GAME_JOIN_H

#include "pair_iterator_stl.h"
#include "sequoia_solver.h"

namespace sequoia {

template <typename Solver>
class ApplyGameJoin {
public:
    ApplyGameJoin<Solver>(Solver *solver,
			       const TreeDecomposition::vertex_descriptor& node, 
			       const Assignment_f *alpha)
    : _solver(solver), _node(node), _alpha(alpha) {
	_bag = _solver->treedecomposition()->bag(node);
        _count = 0UL;
    }
    typedef PairIteratorStl<GameVoidPtrMap::const_iterator, GameVoidPtrMap::const_iterator>::value_type argument_type;
    void init();
    void operator() (const argument_type &entry);
    size_t count() const { return _count; }
private:
    Solver *_solver;
    const TreeDecomposition::vertex_descriptor& _node;
    const Bag* _bag;
    const void *_intersection;
    const Assignment_f *_alpha;
#ifdef HAVE_TBB
    tbb::atomic<size_t> _count;
#else
    size_t _count;
#endif

    const MCGame_f* result(const MCGame_f* game_left, const MCGame_f* game_right);
    void save_value(const MCGame_f* result, const void *value_left, const void *value_right) const;
    ApplyGameJoin<Solver>(const ApplyGameJoin<Solver> &other); // forbid
};

} // namespace

#include "apply_game_join.hpp"

#endif // SEQUOIA_APPLY_GAME_JOIN_H