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
#include "apply_game_root.h"
#include "game.h"

#include <boost/scoped_ptr.hpp>
#ifdef HAVE_TBB
#include <tbb/mutex.h>
#endif

namespace sequoia {

template <typename Solver>
void
ApplyGameRoot<Solver>::operator()(const GameVoidPtrMap::value_type &entry) {
    const MCGame_f *oldgame = entry.first;
    const void *oldvalue = entry.second;
    DPRINTLN("-------------------------------------------------------");
    DPRINT("Next Input: Game" << oldgame->get()->toString());
    DPRINTLN("\twith value: " << _solver->evaluation()->toString(oldvalue));
    DEBUG(oldgame->get()->recursive_print_game(1, true));
    DPRINTLN("");
    const MCGame_f *res = oldgame->get()->convert();
    _count++;
    if (res->get()->outcome() == MCGame::FALSIFIER) {
	DPRINTLN("===> DISCARD");
	delete res;
	return;
    } else {
	DPRINTLN("===> SAVE SOLUTION");
#ifdef HAVE_TBB
	tbb::mutex::scoped_lock lock(*_game_info.mutex);
#endif
	if (!*_game_info.has_solution)
	    *_game_info.solution = oldvalue;
	else
	    *_game_info.solution =
		    _solver->evaluation()->add(*_game_info.solution, oldvalue);
	*_game_info.has_solution = true;
    }
}

} // namespace