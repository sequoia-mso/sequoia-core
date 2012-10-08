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
#include "game_determined.h"

#include <assert.h>

namespace sequoia {

template <>
MCGame::Player MCGame::opponent<MCGame::FALSIFIER>::value = MCGame::VERIFIER;
template <>
MCGame::Player MCGame::opponent<MCGame::VERIFIER>::value = MCGame::FALSIFIER;

DeterminedGameFactory determined;

const MCGame_f* DeterminedGame::convert() const {
    return determined.get(outcome());
}

const MCGame_f* DeterminedGame::forget(const ConstantSymbol* tsym,
				       int signature_depth,
                                       const PointMove* replacement,
                                       const Assignment_f* alpha) const {
    return determined.get(outcome());
}

const MCGame_f* DeterminedGame::introduce(const ConstantSymbol* tsym,
					  int signature_depth,
					  const Assignment_f* alpha) const {
    return determined.get(outcome());
}

const MCGame_f* DeterminedGame::join(const MCGame_f* other,
                                     const Assignment_f* alpha) const {
    return determined.get(outcome());
}

#ifdef DODEBUG
void DeterminedGame::recursive_print_game(int level, bool skip_first) const {
    (skip_first ? std::cout : tab_prefix(level)) << toString() << std::endl;
}
#endif

} // namespace
