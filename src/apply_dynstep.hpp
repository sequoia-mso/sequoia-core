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
#include "apply_dynstep.h"
#include "apply_dynstep_assignments.h"
#include "lightweight_apply.h"

#include <algorithm>
#ifdef HAVE_TBB
#include <tbb/parallel_do.h>
#endif

namespace sequoia {

template <typename Derived, typename ApplyGame, typename Solver>
void ApplyDynStep<Derived, ApplyGame, Solver>::init() {
    const TreeDecomposition* tdc = _solver->treedecomposition();
    _node_bag = tdc->bag(_node);
    _child_bag = tdc->bag(_child);
    DPRINTLN("New Bag: " << *_node_bag << ", Old bag: " << *_child_bag);
    _terminal = find_terminal();
    DPRINTLN("terminal " << _terminal
            << " is node " << vertex_of(_terminal));
    _terminal_sym = _solver->terminal_symbol(_terminal);
    ((Derived*) this)->spec_init();
    setup_assignment();
}

template <typename Derived, typename ApplyGame, typename Solver>
void ApplyDynStep<Derived, ApplyGame, Solver>::setup_assignment() {
    const Assignment_f *child_alpha = _solver->alpha(_child);
    assert(child_alpha != NULL);

    DEBUG({
	std::cout << "child's Terminal Moves are:" << std::endl;
	for (unsigned int i = 0; i < _solver->n_terminals(); ++i) {
	    std::cout << "t_" << i << ": "
		<< TOSTRING(child_alpha->get()->get(_solver->terminal_symbol(i)))
		<< std::endl;
	}
    });

    /* Update the point moves.  We need to rename a few symbols, since the
     * new terminal has been introduced */
    boost::scoped_ptr<const Assignment_f> alpha(_solver->base_alpha()->clone());
    for (unsigned int i = 0; i < _solver->n_terminals(); ++i) {
	const PointMove* newmove = update_pointmove(i);
	alpha.reset(AssignmentFlyFactory::make(
	    new ObjAssignment(alpha.get(), _solver->terminal_symbol(i), newmove)));
    }

    DEBUG({
	std::cout << "new Terminal Moves sind:" << std::endl;
	for (unsigned int i = 0; i < _solver->n_terminals(); ++i) {
	    std::cout << "t_" << i << ": "
		    << TOSTRING(alpha->get()->get(_solver->terminal_symbol(i)))
		    << std::endl;
	}
    });

    _solver->alpha(_node, alpha.get());
}

template <typename Derived, typename ApplyGame, typename Solver>
void ApplyDynStep<Derived, ApplyGame, Solver>::operator()() {
    ApplyDynStepAssignment<ApplyGame, Solver> apply(_solver, _node, _terminal_sym, _game_info);
    DPRINTLN("Start iterating over assignments...");
    SequoiaTable *tab = _solver->table(_child);
#ifdef HAVE_TBB
    tbb::parallel_do(tab->begin(), tab->end(), lightweight_apply(&apply));
#else
    std::for_each(tab->begin(), tab->end(), lightweight_apply(&apply));
#endif
}

} // namespace
