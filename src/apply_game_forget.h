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
#ifndef SEQUOIA_APPLY_GAME_FORGET_H
#define SEQUOIA_APPLY_GAME_FORGET_H

#include "sequoia_solver.h"

namespace sequoia {

template <typename Solver>
class ApplyGameForget {
public:
    typedef struct {
        const PointMove *replacement;
    } ApplyGameInfo;
    ApplyGameForget<Solver>(Solver *solver,
			       const TreeDecomposition::vertex_descriptor& node, 
		               const ConstantSymbol* forgotten_ts,
			       const ApplyGameInfo &game_info,
			       const Assignment_f *child_alpha)
    : _solver(solver), _node(node), _forgotten_ts(forgotten_ts), _alpha(NULL),
      _child_alpha(child_alpha), _game_info(game_info) {
	_bag = _solver->treedecomposition()->bag(node);
        _count = 0UL;
    }
    ~ApplyGameForget<Solver>() {
	delete _alpha;
    }
    void init();
    typedef GameVoidPtrMap::value_type argument_type;
    void operator() (const argument_type &entry);
    size_t count() const { return _count; }
private:
    Solver *_solver;
    const TreeDecomposition::vertex_descriptor& _node;
    const ConstantSymbol *_forgotten_ts;
    const Assignment_f *_alpha;
    const Assignment_f *_child_alpha;
    ApplyGameInfo _game_info;
    const Bag* _bag;
    const PointMove *_replacement;
#ifdef HAVE_TBB
    tbb::atomic<size_t> _count;
#else
    size_t _count;
#endif

    const MCGame_f* result(const MCGame_f* oldgame);
    ApplyGameForget<Solver>(const ApplyGameForget<Solver> &other); // forbid
};

} // namespace

#include "apply_game_forget.hpp"

#endif // SEQUOIA_APPLY_GAME_FORGET_H