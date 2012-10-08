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
#include "temp_symbol_factory.h"

#include <boost/smart_ptr/scoped_ptr.hpp>

namespace sequoia {

template <class Derived, class TMove>
typename QUndetGameForgetIteratorBase<Derived, TMove>::value_type
QUndetGameForgetIteratorBase<Derived, TMove>::next() {
    const TMove* oldmove;
    const MCGame_f* oldgame;
    boost::tie(oldmove, oldgame) = game_iterator().next();
    has_next(game_iterator().has_next());
    DEBUG(tab_prefix(level()) << "GameIterator returns pair :"
	    << TOSTRING(oldmove) << " / "
	    << oldgame->get()->toString() << std::endl);

    const TMove* modified = NULL;
    const PointMove *my_replacement = NULL;

    if (_last_oldmove != NULL && _last_oldmove == oldmove) {
	// already adjusted the move in the last round, use that
	modified = _last_modified;
        my_replacement = _last_replacement;
    } else {
        // First, adjust the forgotten terminal symbol
        if (replacement()->nesting_depth() < variable()->nesting_depth()) {
            /* We already inserted the forgotten terminal as an anonymous
             * point move in an earlier level of the recursion.  Use this one. */
            my_replacement = replacement();
        } else {
            /* The forgotten terminal symbol is to be replaced below in the recursion
             * tree.  Increase the nesting_depth of the replacement symbol. */
	    my_replacement = this->adjust_replacement(oldmove);
        }
        // Second, adjust the old set move
	modified = this->adjust_move(oldmove);
    }

    boost::scoped_ptr<const Assignment_f> my_alpha(AssignmentFlyFactory::make(
        new SymAssignment<typename TMove::symbol_type, TMove>(alpha(), variable(),modified)));
    DEBUG(tab_prefix(level()) << "Modif: " << TOSTRING(modified) << std::endl);

#if USE_CACHE_SUBGAMES
    const MCGame_f* sub = cache_forget_lookup(oldgame->get()->formula(),
                                              my_alpha.get(), oldgame, tsym());
#else
    const MCGame_f *sub = NULL;
#endif
    if (sub == NULL) {
        sub = oldgame->get()->forget(tsym(),
                                     signature_depth(),
                                     my_replacement,
                                     my_alpha.get());
#if USE_CACHE_SUBGAMES
        cache_forget_store(oldgame->get()->formula(),
                           my_alpha.get(), oldgame, tsym(), sub);
#endif
    }

    _last_oldmove = oldmove;
    _last_modified = modified;
    _last_replacement = my_replacement;
    return std::make_pair(modified, sub);
}

}; // namespace