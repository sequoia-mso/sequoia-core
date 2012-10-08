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
#include "game_q.h"
#include "moves_pool.h"

namespace sequoia {

QUndetGameIntroduceIterator<SetMove>::value_type
QUndetGameIntroduceIterator<SetMove>::next() {
    // if setbit is false; we need to get the next game from the game iterator
    if (!_setbit) {
	boost::tie(_cur_move, _cur_game) = game_iterator().next();
	DEBUG(tab_prefix(level()) << TOSTRING(_cur_move) << std::endl);
	// << _cur_game->get()->toString() << std::endl);
	has_next(true);
    }
    
    // now adjust the moves
    const SetMove* modified = NULL;
    if (_cur_move->nesting_depth() < variable()->nesting_depth()) {
	// If the oldmove's global nesting depth is smaller then the nesting
	// depth of the current formula, we already modified the move in
	// a previous step.   Just look up this modified interpretation by
	// the correct nesting depth. 
	modified = alpha()->get()->get(_cur_move->symbol());
    } else {
	// The previous move is on the correct global nesting depth,
	// but the terminals have been renamed.
	SetMove* newmove = new SetMove(*_cur_move);
	newmove->introduce(tsym(), _setbit, signature_depth());
	modified = moves_pool.pool(newmove);
    }
    boost::scoped_ptr<const Assignment_f> my_alpha(AssignmentFlyFactory::make(
        new SetAssignment(alpha(), variable(), modified)));
    DEBUG(tab_prefix(level()) << "Modif: " << TOSTRING(modified) << std::endl);
    const MCGame_f* sub = _cur_game->get()->introduce(tsym(),
						      signature_depth(),
						      my_alpha.get());
    if (_setbit) // this time we need to ask the game iterator if there are more
	has_next(game_iterator().has_next());
    // toggle setbit for next round
    _setbit = !_setbit;
    return std::make_pair(modified, sub);
}

QUndetGameIntroduceIterator<PointMove>::value_type
QUndetGameIntroduceIterator<PointMove>::next() {
    if (_repeat_last_move) {
#if 0
	DEBUG(tab_prefix(level()) << "Repeating last game with NULL move: "
		<< _last_game->get()->toString() << std::endl);
#endif
	_repeat_last_move = false;
	const PointMove* tmove = alpha()->get()->get(tsym());
	DEBUG(tab_prefix(level()) << "(NULL) -mod-> " << TOSTRING(tmove)
		<< " - point move on new terminal: " << std::endl);
	assert(tmove != NULL);
	boost::scoped_ptr<const Assignment_f> my_alpha(AssignmentFlyFactory::make(
                new ObjAssignment(alpha(), variable(), tmove)));
	const MCGame_f* sub = _last_game->get()->introduce(tsym(),
							 signature_depth(),
							 my_alpha.get());
	has_next(game_iterator().has_next());
	return std::make_pair(tmove, sub);
    }
    const PointMove* oldmove;
    const MCGame_f* oldgame;
    boost::tie(oldmove, oldgame) = game_iterator().next();

    DEBUG(tab_prefix(level()) << TOSTRING(oldmove));
    // << " / " << oldgame->get()->toString() << std::endl);
    has_next(game_iterator().has_next());
    
    const PointMove* modified = NULL;
    if (oldmove == NULL) {
	// no changes required for the move
    } else if (oldmove->nesting_depth() < variable()->nesting_depth()) {
	// If the old move's nesting depth is smaller than the nesting
	// depth of the current formula, we already modified the move in
	// a previous step.   Just look up this modified interpretation by
	// the correct nesting depth.
	if (oldmove->nesting_depth() < tsym()->nesting_depth()) {
	    // If the old nesting depth is smaller than the new
	    // terminal's nesting depth, nothing has changed.
	    modified = alpha()->get()->get(oldmove->symbol());
	} else if (oldmove->nesting_depth() >= tsym()->nesting_depth() && oldmove->terminal()) {
	    // The old move was onto a terminal, but this terminal
	    // has already been renamed due to the introduce step; add the +1 offset.
	    // The Move itself has already been fixed in interpretation
	    int old_depth = oldmove->nesting_depth();
	    modified = alpha()->get()->get_pointmove(old_depth+1);
	} else {
	    // the move is not a terminal, so we can safely use the
	    // previous move's symbol to look up the new, alerady modified interpretation
	    modified = alpha()->get()->get(oldmove->symbol());
        }
    } else {
	// The previous move is on the correct global nesting depth,
	// but the terminals have been renamed.
	PointMove* newmove = new PointMove(*oldmove);
	newmove->introduce(tsym(), signature_depth());
	// no need to update adjaceny:  non-terminal nodes as this one are
	// never adjacent to this newly introduced terminal.
	assert(newmove->nesting_depth() == oldmove->nesting_depth());
	modified = moves_pool.pool(newmove);
    }
    DPRINTLN(" -mod-> " << TOSTRING(modified) << ":");
    boost::scoped_ptr<const Assignment_f> my_alpha(AssignmentFlyFactory::make(
        new ObjAssignment(alpha(), variable(), modified)));
    const MCGame_f* sub = oldgame->get()->introduce(tsym(),
						    signature_depth(),
						    my_alpha.get());

    if (oldmove == NULL) {
	_repeat_last_move = true;
	has_next(true);
    }
    _last_move = oldmove;
    _last_game = oldgame;
    return std::make_pair(modified, sub);
}

}; // namespace