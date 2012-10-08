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
#include "apply_dynstep_assignments.h"
#include "lightweight_apply.h"

#include <algorithm>
#include <boost/scoped_ptr.hpp>
#ifdef HAVE_TBB
#include <tbb/parallel_do.h>
#endif

namespace sequoia {

template <typename ApplyGame, typename Solver>
void
ApplyDynStepAssignment<ApplyGame, Solver>::operator()(const SequoiaTable::const_iterator::value_type &entry) {
    const Assignment_f *child_alpha = entry.first;
    const GameVoidPtrMap *inmap = entry.second;
    ApplyGame apply(_solver, _node, _terminal_symbol, _game_info, child_alpha);
    apply.init();
    DPRINTLN("Start iterating over sets");
    GameVoidPtrMap::const_iterator git = inmap->begin();
    GameVoidPtrMap::const_iterator gitend = inmap->end();
#ifdef HAVE_TBB
    tbb::parallel_do(git, gitend, lightweight_apply(&apply));
#else
    std::for_each(git, gitend, lightweight_apply(&apply));
#endif
    _solver->log_games_completed(apply.count());
}

} // namespace
