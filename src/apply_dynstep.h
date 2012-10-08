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
#ifndef SEQUOIA_APPLY_DYNSTEP_H
#define SEQUOIA_APPLY_DYNSTEP_H

#include "sequoia_table.h"

namespace sequoia {

template <typename Derived, typename ApplyGame, typename Solver>
class ApplyDynStep {
public:
    ApplyDynStep<Derived, ApplyGame, Solver>(
	Solver *solver,
	const TreeDecomposition::vertex_descriptor& node,
	const TreeDecomposition::vertex_descriptor& child
      )
      : _solver(solver), _node(node), _child(child), _game_info() { }
    void init();
    void operator()();
protected:
    const PointMove* update_pointmove(unsigned int i) const {
	return ((const Derived*) this)->update_pointmove_impl(i);
    }
    Solver *_solver;
    const TreeDecomposition::vertex_descriptor &_node;
    const TreeDecomposition::vertex_descriptor &_child;
    const Bag* _node_bag;
    const Bag* _child_bag;
    unsigned int _terminal;
    const ConstantSymbol *_terminal_sym;
    typedef typename ApplyGame::ApplyGameInfo ApplyGameInfo;
    ApplyGameInfo _game_info;
private:
    void setup_assignment();
    unsigned int find_terminal() {
	if (_node_bag->width() > _child_bag->width())
	    return _child_bag->missing_terminal(*_node_bag);
	else
	    return _node_bag->missing_terminal(*_child_bag);
    }
    unsigned int vertex_of(int t) {
	if (_node_bag->width() > _child_bag->width())
	    return _node_bag->get(t);
	else
	    return _child_bag->get(t);
    }
    // forbid
    ApplyDynStep<Derived, ApplyGame, Solver>(const ApplyDynStep<Derived, ApplyGame, Solver> &);
};

} // namespace

#include "apply_dynstep.hpp"

#endif //  SEQUOIA_APPLY_DYNSTEP_H