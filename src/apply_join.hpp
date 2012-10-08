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
#include "apply_join.h"
#include "apply_join_assignments.h"
#include "lightweight_apply.h"

#include <algorithm>
#ifdef HAVE_TBB
#include <tbb/parallel_do.h>
#endif

namespace sequoia {

template <typename Solver>
void ApplyJoin<Solver>::init() {
    _solver->alpha(_node, _solver->alpha(_child_left)->clone());
}

template <typename Solver>
void ApplyJoin<Solver>::operator()() {
    ApplyJoinAssignment<Solver> apply(_solver, _node, _child_left, _child_right);
    DPRINTLN("Start iterating over assignments...");
    SequoiaTable *tab = _solver->table(_child_left);
#ifdef HAVE_TBB
    tbb::parallel_do(tab->begin(), tab->end(), lightweight_apply(&apply));
#else
    std::for_each(tab->begin(), tab->end(), lightweight_apply(&apply));
#endif
}

} // namespace
