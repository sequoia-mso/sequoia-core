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
#include "apply_forget.h"
#include "moves_pool.h"
#include "temp_symbol_factory.h"

namespace sequoia {

/**
 * Save the forgotten terminal's neighbors and bound set moves memberships
 * so that we can use them when creating the anonymous point move later.
 */
template <typename Solver>
void ApplyForget<Solver>::spec_init() {
    const Assignment_f *child_alpha = Base::_solver->alpha(Base::_child);

    // create a new anonymous point move replacing the terminal
    unsigned int depth = Base::_solver->vocabulary()->size();
    PointMove *tmp_replacement = new PointMove(create_temporary_symbol(depth));

    //  bound unary symbols
    const GraphStructure* structure = Base::_solver->graph();
    const Vocabulary* gvoc = structure->vocabulary();
    for (unsigned int i = 0; i < gvoc->number_of_unary_symbols(); ++i) {
	const UnarySymbol* sym = gvoc->unary_symbol(i);
	assert(sym != NULL);
        if (structure->has_label(Base::_child_bag->get(Base::_terminal), sym))
            tmp_replacement->add_label(sym);
    }
    // neighbors
    for(unsigned int i = 0; i < Base::_child_bag->width(); ++i) {
	if (i < Base::_terminal) {
	    const ConstantSymbol *tsym = Base::_solver->terminal_symbol(i);
	    const PointMove *tmove = child_alpha->get()->get(tsym);
            if (tmove->test_edge(tsym))
                tmp_replacement->add_edge(tsym);
	}
	if (i > Base::_terminal) {
	    const ConstantSymbol *oldtsym = Base::_solver->terminal_symbol(i);
	    const ConstantSymbol *newtsym = Base::_solver->terminal_symbol(i-1);
	    const PointMove *tmove = child_alpha->get()->get(oldtsym);
            if (tmove->test_edge(oldtsym))
                tmp_replacement->add_edge(newtsym);
	}
    }
    Base::_game_info.replacement = moves_pool.pool(new PointMove(*tmp_replacement));
}

template <typename Solver>
const PointMove*
ApplyForget<Solver>::update_pointmove_impl(unsigned int i) const {
    const Assignment_f *child_alpha = Base::_solver->alpha(Base::_child);
    if (i < Base::_terminal) {
	// nothing changed, terminal i is still symbol t_i...
	return child_alpha->get()->get(Base::_solver->terminal_symbol(i));
    } if (i < Base::_solver->n_terminals() - 1) {
	// terminal i was terminal i+1
	const PointMove* oldmove = child_alpha->get()->get(Base::_solver->terminal_symbol(i+1));
	if (oldmove == NULL) // no changes required
	    return oldmove;
	PointMove *tmp = oldmove->rename_forget(Base::_terminal_sym,
					 Base::_solver->terminal_symbol(i));
	return moves_pool.pool(tmp);
    } else {
	// the last terminal is NULL
	return NULL;
    }

}

} // namespace