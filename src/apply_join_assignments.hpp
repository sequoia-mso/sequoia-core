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
#include "apply_join_assignments.h"
#include "apply_game_join.h"
#include "lightweight_apply.h"
#include "pair_iterator_stl.h"

#include <algorithm>
#include <boost/scoped_ptr.hpp>
#ifdef HAVE_TBB
#include <tbb/parallel_do.h>
#endif

namespace sequoia {

template <typename Solver>
void
ApplyJoinAssignment<Solver>::operator()(const SequoiaTable::const_iterator::value_type &entry) {
    const Assignment_f *alpha = entry.first;
    const GameVoidPtrMap *left_inmap = entry.second;

    SequoiaTable *right_table = _solver->table(_child_right);
    SequoiaTable::const_iterator ralpha_it = right_table->find(alpha);
    if (ralpha_it == right_table->end()) {
	// no compatible entries in the right table, no need to iterate.
	return;
    }
    assert(*alpha == *ralpha_it->first);
    const GameVoidPtrMap *right_inmap = ralpha_it->second;
    ApplyGameJoin<Solver> apply(_solver, _node, alpha);
    apply.init();
    
    /*
     * Now we iterate over all games in a Cartesian manner and for each
     * pair we join the two games with each other.
     */
    typedef PairIteratorStl<GameVoidPtrMap::const_iterator, GameVoidPtrMap::const_iterator> PairIt;
    PairIt pit(left_inmap->begin(), left_inmap->end(),
	       right_inmap->begin(), right_inmap->end());
    PairIt pitend(left_inmap->end(), right_inmap->end());
    DPRINTLN("Start iterating over sets");
#ifdef HAVE_TBB
    tbb::parallel_do(pit, pitend, lightweight_apply(&apply));
#else
    std::for_each(pit, pitend, lightweight_apply(&apply));
#endif
    _solver->log_games_completed(apply.count());
}

} // namespace
