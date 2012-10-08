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
#include "cache_forget.h"
#include "game_q_forget_iterator.h"
#include "moves_pool.h"

namespace sequoia {

const PointMove*
QUndetGameForgetIterator<SetMove>::adjust_replacement(const SetMove *oldmove) {
    /* Introduce a temporary symbol with the correct nesting depth */
    bool member = oldmove->test(tsym());
    const ConstantSymbol *tmp_symbol = create_temporary_symbol(variable()->nesting_depth() + 1);
    return moves_pool.pool(replacement()->rename_introduce(variable(), tmp_symbol, member));
}

const PointMove*
QUndetGameForgetIterator<PointMove>::adjust_replacement(const PointMove *oldmove) {
    /* The forgotten terminal symbol is to be replaced below in the recursion
     * tree.  Increase its nesting_depth by introducing the current variable() */
    bool adjacent = false;
    /* Check whether the current move is on the same level as the variable.
     * If so, we need to save its label */
    if (oldmove != NULL && oldmove->nesting_depth() == replacement()->nesting_depth())
	adjacent = oldmove->test_edge(tsym());
    const ConstantSymbol *tmp_symbol = create_temporary_symbol(variable()->nesting_depth() + 1);
    return moves_pool.pool(replacement()->rename_introduce(variable(), tmp_symbol, adjacent));
}

const SetMove*
QUndetGameForgetIterator<SetMove>::adjust_move(const SetMove *oldmove) {

    if (oldmove->nesting_depth() < variable()->nesting_depth()) {
	/* In this case, we already modified the move in a previous level
	 * of the recursion and saved it in the current alpha.  Just look
	 * up this modified alpha, which is possible since we never
	 * rename unary symbols */
	return alpha()->get()->get(oldmove->symbol());
    } else {
	/* Otherwise we need to rename the terminal symbols. */
	SetMove* newmove = new SetMove(*oldmove);
	newmove->forget(tsym(), signature_depth());
	return moves_pool.pool(newmove);
    }
}

const PointMove*
QUndetGameForgetIterator<PointMove>::adjust_move(const PointMove *oldmove) {
    // Second, adjust the old point move
    if (oldmove == NULL) {
	// No changes required for the NULL move.
	return oldmove;
    }
    if (oldmove->nesting_depth() == tsym()->nesting_depth()) {
	/* This is the move that is being forgotten, i.e., we have a
	 * PointMove to this terminal.  It needs to be replaced with 
	 *  the anonymous pointmove replacement.  There are two cases:
	 * - either we already replaced the symbol in a higher level
	 *   of the recursion tree.  In this case, we simly use replaced().
	 * - or we need to use the replacement() move, but need to
	 *   replace its constant symbol to match the current variable.
	 **/
	if (replacement()->nesting_depth() < variable()->nesting_depth()) {
	    // already replace before, use that one
	    return replacement();
	} else {
	    // adjust the pointmove
	    PointMove *tmp = new PointMove(*replacement());
	    tmp->symbol(variable());
	    return moves_pool.pool(tmp);
	}
    } else if (oldmove->nesting_depth() < variable()->nesting_depth()) {
	/* In this case, we already modified the move in a previous level
	 * of the recursion and saved it in the current alpha.  Just look
	 * up this modified alpha by the correct (new) nesting depth.
	 * There are three possitions cases where we have to look it up.
	 */
	if (oldmove->nesting_depth() < tsym()->nesting_depth()) {
	    /* If the old nesting depth is even smaller than the forgotten
	     * terminal's nesting depth, the symbol has not been renamed. */
	    return alpha()->get()->get(oldmove->symbol());
	} else if (oldmove->nesting_depth() >= tsym()->nesting_depth() && oldmove->terminal()) {
	    /* The old move was onto a terminal, but this terminal
	     * has been renamed due to the forget step; add the -1 offset. */
	    int old_depth = oldmove->nesting_depth();
	    return alpha()->get()->get_pointmove(old_depth - 1);
	} else {
	    /* The move is not a terminal, so we can safely use the
	     * previous's symbol to look up the new, already modifeid alpha.
	     * This works because we neve rename variables */
	    return alpha()->get()->get(oldmove->symbol());
	}
    } else {
	/* The previous move is on the correct global nesting depth, 
	 * but the terminals have been renamed.
	 */
	PointMove* newmove = new PointMove(*oldmove);
	newmove->forget(tsym(), signature_depth());
	assert(newmove->nesting_depth() == oldmove->nesting_depth());
	return moves_pool.pool(newmove);
    } 
}

}; // namespace