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
#include "sequoia_table.h"

namespace sequoia {

void
SequoiaTable::update_value(const Assignment_f* alpha,
			   const MCGame_f* game,
			   const void *val) {
    assert(game != NULL);
    if (game->get()->outcome() == MCGame::FALSIFIER) {
	delete game;
	return;
    }
    GameVoidPtrMap *map = new GameVoidPtrMap();
    const Assignment_f *my_alpha = alpha->clone();
    std::pair<Container::iterator, bool> res = _container.insert(std::make_pair(my_alpha, map));
    if (!res.second) { // entry exists already
	delete my_alpha;
	delete map;
    }
    map = res.first->second;
    map->update_value(game, val, _evaluation);
}

} // namespace