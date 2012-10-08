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
#ifndef SEQUOIA_APPLY_DYNSTEP_ASSIGNMENTS_H
#define SEQUOIA_APPLY_DYNSTEP_ASSIGNMENTS_H

#include "sequoia_solver.h"

namespace sequoia {

template <typename ApplyGame, typename Solver>
class ApplyDynStepAssignment {
public:
    typedef typename ApplyGame::ApplyGameInfo ApplyGameInfo;
    ApplyDynStepAssignment<ApplyGame, Solver>(Solver *solver,
	const TreeDecomposition::vertex_descriptor& node,
	const ConstantSymbol *terminal_symbol,
	const ApplyGameInfo &game_info)
    : _solver(solver), _node(node), _terminal_symbol(terminal_symbol),
      _game_info(game_info) { }
    typedef SequoiaTable::const_iterator::value_type argument_type;
    void operator() (const argument_type &entry);
private:
    Solver *_solver;
    const TreeDecomposition::vertex_descriptor& _node;
    const ConstantSymbol *_terminal_symbol;
    const ApplyGameInfo &_game_info;
    // forbid
    ApplyDynStepAssignment<ApplyGame, Solver>(const ApplyDynStepAssignment<ApplyGame, Solver> &);
};

} // namespace

#include "apply_dynstep_assignments.hpp"

#endif // SEQUOIA_APPLY_DYNSTEP_ASSIGNMENTS_H