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
#include "apply_game_join.h"
#include "cache_join.h"
#include "moves_pool.h"

#include <boost/scoped_ptr.hpp>

namespace sequoia {

template <typename Solver>
void
ApplyGameJoin<Solver>::init() {
    _intersection = _solver->evaluation()->elem(_bag, _alpha->get());
}

template <typename Solver>
const MCGame_f *
ApplyGameJoin<Solver>::result(const MCGame_f* game_left, const MCGame_f* game_right) {
    const MCGame_f *cached = cache_join_lookup(game_left->get()->formula(),
                                               _alpha, game_left, game_right);
    if (cached != NULL) {
	DPRINTLN("Join Cache hit");
	DPRINTLN("Cache result: " << cached->get()->toString());
        DEBUG(cached->get()->recursive_print_game(0, true));
	return cached;
    }
    const MCGame_f* resgame = game_left->get()->join(game_right, _alpha);
    DPRINTLN("Result:");
    DEBUG(resgame->get()->recursive_print_game(0, true));
    DPRINTLN("");
    cache_join_store(game_left->get()->formula(), _alpha, game_left, game_right, resgame);
    return resgame;
}

template <typename Solver>
void
ApplyGameJoin<Solver>::operator()(const argument_type &entry) {
    const MCGame_f *game_left = entry.first.first;
    const MCGame_f *game_right = entry.second.first;
    const void *value_left = entry.first.second;
    const void *value_right = entry.second.second;
    
    DPRINTLN("-------------------------------------------------------");
    DPRINT("Next Left Game:  " << game_left->get()->toString());
    DPRINTLN("\twith value: " << _solver->evaluation()->toString(value_left) << " and game:");
    DEBUG(game_left->get()->recursive_print_game(1, true));
    DPRINT("Next Right Game: " << game_right->get()->toString());
    DPRINTLN("\twith value: " << _solver->evaluation()->toString(value_right) << " and game:");
    DEBUG(game_right->get()->recursive_print_game(1, true));

    const MCGame_f *res = result(game_left, game_right);
    save_value(res, value_left, value_right);
    _count++;
}

/**
 * deletes the result game, do not delete elsewhere
 */
template <typename Solver>
void
ApplyGameJoin<Solver>::save_value(const MCGame_f* result, const void* value_left, const void* value_right) const {
    if (result->get()->outcome() == MCGame::FALSIFIER) {
	delete result;
	return;
    }
    const void *val = _solver->evaluation()->mult(value_left, value_right, _intersection);
    _solver->table(_node)->update_value(_alpha, result, val);
}

} // namespace
