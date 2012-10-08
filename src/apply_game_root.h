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
#ifndef SEQUOIA_APPLY_GAME_ROOT_H
#define SEQUOIA_APPLY_GAME_ROOT_H

#include "sequoia_solver.h"

#ifdef HAVE_TBB
#include <tbb/mutex.h>
#endif

namespace sequoia {

template <typename Solver>
class ApplyGameRoot {
public:
    class ApplyGameInfo {
    public:
	ApplyGameInfo()
	: has_solution(new bool()), solution(new const void*())
#ifdef HAVE_TBB
	  ,mutex(new tbb::mutex())
#endif
	{ }
	~ApplyGameInfo() {
	    delete has_solution;
	    delete solution;
#ifdef HAVE_TBB
	    delete mutex;
#endif
	}
	bool *has_solution;
	const void **solution;
#ifdef HAVE_TBB
	tbb::mutex *mutex;
#endif
    };
    ApplyGameRoot<Solver>(Solver *solver,
			  const TreeDecomposition::vertex_descriptor& node,
			  const ConstantSymbol*,
			  const ApplyGameInfo &game_info,
	                  const Assignment_f *)
    : _solver(solver), _node(node), _game_info(game_info) {
	_bag = _solver->treedecomposition()->bag(node);
        _count = 0UL;
    }
    void init() { }
    typedef GameVoidPtrMap::value_type argument_type;
    void operator() (const argument_type &entry);
    size_t count() const { return _count; }
private:
    Solver *_solver;
    const TreeDecomposition::vertex_descriptor& _node;
    const Bag* _bag;
    const ApplyGameInfo &_game_info;
#ifdef HAVE_TBB
    tbb::atomic<size_t> _count;
#else
    size_t _count;
#endif

    ApplyGameRoot<Solver>(const ApplyGameRoot<Solver> &); // forbid
};

} // namespace

#include "apply_game_root.hpp"

#endif // SEQUOIA_APPLY_GAME_ROOT_H