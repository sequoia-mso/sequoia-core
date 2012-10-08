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
#include "cache_join.h"
#include "game_determined.h"
#include "game_q.h"
#include "game_q_introduce_iterator.h"
#include "game_q_forget_iterator.h"
#include "pair_iterator.h"

#include <boost/scoped_ptr.hpp>

namespace sequoia {

template <typename TFormula, typename TMove, MCGame::Player _player>
const MCGame_f* QUndetGame<TFormula, TMove, _player>::minimize() const {
    assert(outcome() == MCGame::UNDETERMINED);
    if (empty()) {
    	delete this;
    	return determined.get(MCGame::opponent<_player>::value);
    } else {
        return MCGameFlyFactory::make(this);
    }
}

template <typename TFormula, typename TMove, MCGame::Player _player> 
void QUndetGame<TFormula, TMove, _player>::set_subgame(const TMove *move,
						       const MCGame_f* game) {
    bool res = _subgames.insert(move, game);
    if (!res) { // new entry
        // equivalent entry exists already, delete this game.
        delete game;
    }
}

template <typename TFormula, typename TMove, MCGame::Player _player>
const MCGame_f* QUndetGame<TFormula, TMove, _player>::add_subgame(const TMove* move,
								  const MCGame_f* game) {
    if (game->get()->outcome() == _player) {
	if (is_pointmove<TMove>::value && move == NULL) { // compiler will optimize this away for non-pointmoves
	    /*
	     * The current player must not use the NULL move for their advantage.
	     * An example where this may happen is testing whether the graph is
	     * a complete graph:  ALL x ALL y adj(x, y).
	     * If there is a non-vertex in the PAST, the game for adj(x,y)
	     * will return FALSE for the NULL move (due to the graph separator
	     * property).  Returning FALSE here would be wrong, when there is
	     * no vertex left to come.  We have to wait until this move is concrete.
	     */
	    DPRINTLN(" --> re-check this "
		    << formula()->variable()->identifier()
		    << " later (when concrete)!");
	    QUndetGame<TFormula, TMove, _player>::set_subgame(NULL, game);
	    return NULL;
	} else {
                DPRINTLN(" --> determines!");
                delete this;
		return game;
	}
    }
    if (game->get()->outcome() == MCGame::opponent<_player>::value) {
	    DPRINTLN(" --> ignore this "
		    << formula()->variable()->identifier()<< "!");
	    delete game;
	    return NULL;
    }
    DPRINTLN(" --> re-check this "
            << formula()->variable()->identifier() << " later!");
    QUndetGame<TFormula, TMove, _player>::set_subgame(move, game);
    return NULL;
}

template <typename TFormula, typename TMove, MCGame::Player _player>
const MCGame_f* QUndetGame<TFormula, TMove, _player>::convert() const {
    DEBUG({
        tab_prefix(level()) << "Building for game: "
                << toString() << std::endl;
    });
    QUndetGame<TFormula, TMove, _player>* returngame = new QUndetGame<TFormula, TMove, _player>(formula());
    typename QUndetGame<TFormula, TMove, _player>::GamesContainer::GameIterator
    	it(QUndetGame<TFormula, TMove, _player>::begin(),
	   QUndetGame<TFormula, TMove, _player>::end());
    while (it.has_next()) {
	std::pair<const TMove*, const MCGame_f*> res = it.next();
	const TMove* oldmove = res.first;
	const MCGame_f* oldgame = res.second;
        //DEBUG(tab_prefix(level()) << TOSTRING(res.first) << " --> " << res.second->get()->toString());
        DEBUG(tab_prefix(level()) << "Found Game for: " << TOSTRING(oldmove) << std::endl);
	DEBUG(tab_prefix(level()) << "Found Game " << oldgame->get()->toString() << std::endl);
	if (oldmove == NULL) continue; // this game is to be cut
	const MCGame_f* sub = oldgame->get()->convert();
	DEBUG(tab_prefix(level()) << TOSTRING(oldmove) << " --> " << sub->get()->toString());
        const MCGame_f* tmpgame = returngame->add_subgame(res.first, sub);
        if (tmpgame != NULL) return tmpgame;
    }
    return returngame->minimize();
}

template <typename TFormula, typename TMove, MCGame::Player _player>
const MCGame_f* QUndetGame<TFormula, TMove, _player>::forget(const ConstantSymbol* tsym,
							     int signature_depth,
							     const PointMove* replacement,
							     const Assignment_f* alpha) const {
    DEBUG({tab_prefix(level()) << "Building for game: " << TOSTRING(this) << std::endl;});

    QUndetGame<TFormula, TMove, _player>* returngame = new QUndetGame<TFormula, TMove, _player>(formula());
    DEBUG(returngame->level(level()));
    QUndetGameForgetIterator<TMove> it(tsym,
				       signature_depth,
				       replacement,
				       alpha,
				       formula()->variable(),
				       QUndetGame<TFormula, TMove, _player>::begin(),
				       QUndetGame<TFormula, TMove, _player>::end()
				       );
    DEBUG(it.level(level()));
    while (it.has_next()) {
	const std::pair<const TMove*, const MCGame_f*> res = it.next();
        DEBUG(tab_prefix(level()) << TOSTRING(res.first) << " --> " << res.second->get()->toString());
        const MCGame_f* tmpgame = returngame->add_subgame(res.first, res.second);
        if (tmpgame != NULL) return tmpgame;
    }
    return returngame->minimize();
}

template <typename TFormula, typename TMove, MCGame::Player _player>
const MCGame_f* QUndetGame<TFormula, TMove, _player>::introduce(const ConstantSymbol* tsym,
								int signature_depth,
								const Assignment_f* alpha) const {
    DEBUG({ tab_prefix(level()) << "Building for game: " << TOSTRING(this) << std::endl; });
    QUndetGame<TFormula, TMove, _player>* returngame = new QUndetGame<TFormula, TMove, _player>(formula());
    DEBUG(returngame->level(level()));
    QUndetGameIntroduceIterator<TMove> it(tsym,
			 signature_depth,
			 alpha,
			 formula()->variable(),
			 QUndetGame<TFormula, TMove, _player>::begin(),
			 QUndetGame<TFormula, TMove, _player>::end()
			 );
    DEBUG(it.level(level()));
    while (it.has_next()) {
	const std::pair<const TMove*, const MCGame_f*> res = it.next();
        DEBUG(tab_prefix(level()) << TOSTRING(res.first) << " --> " << res.second->get()->toString());
        const MCGame_f* tmpgame = returngame->add_subgame(res.first, res.second);
        if (tmpgame != NULL) return tmpgame;
    }
    return returngame->minimize();
}

template <typename TFormula, typename TMove, MCGame::Player _player>
const MCGame_f* QUndetGame<TFormula, TMove, _player>::join(const MCGame_f* other,
							   const Assignment_f* alpha) const {
    DEBUG({
        tab_prefix(level()) << "Building for game: "
                << toString() << std::endl;
    });
    if (other->get()->outcome() != MCGame::UNDETERMINED) {
        DEBUG(tab_prefix(level()) << "Found determined game, return." << std::endl);
        return other->clone();
    }
    return QUndetGame<TFormula, TMove, _player>::join_impl(other, alpha, tag<TMove>());
}

template <typename TFormula, typename TMove, MCGame::Player _player>
const MCGame_f* QUndetGame<TFormula, TMove, _player>::join_impl(const MCGame_f* other,
							   const Assignment_f* alpha,
							   tag<SetMove>) const {
    typedef QUndetGame<TFormula, TMove, _player> MyType;
    typedef PairIterator<subgames_iterator, subgames_iterator> GamePairIterator;
    typedef typename GamePairIterator::value_type GamePairIteratorValue;
    const MyType* gother = static_cast<const MyType*>(other->get());
    MyType* returngame = new MyType(formula());
    DEBUG(returngame->level(level()));
    
    const_iterator mit = begin();
    const_iterator mitend = end();
    for (; mit != mitend; mit++) {
	const SetMove* oldmove = mit->first;
        DEBUG(tab_prefix(level()) << "Found Games for: " << TOSTRING(oldmove) << std::endl);

	boost::scoped_ptr<const Assignment_f> my_alpha(AssignmentFlyFactory::make(
                new SetAssignment(alpha, formula()->variable(), oldmove)));

	const_iterator mit2 = gother->find(oldmove);
	if (mit2 == gother->end()) {
	    DEBUG(tab_prefix(level()) << TOSTRING(oldmove) << " --> ignored in other, skip" << std::endl);
            continue;
	}
	GamePairIterator git(mit->second->begin(), mit->second->end(),
	                     mit2->second->begin(), mit2->second->end());
	while (git.has_next()) {
	    GamePairIteratorValue res = git.next();
	    const MCGame_f* g1 = res.first;
	    const MCGame_f* g2 = res.second;
#if USE_CACHE_SUBGAMES
	    const MCGame_f *sub = cache_join_lookup(g1->get()->formula(), my_alpha.get(), g1, g2);
#else
	    const MCGame_f *sub = NULL;
#endif
	    if (sub == NULL) {
		sub = g1->get()->join(g2, my_alpha.get());
#if USE_CACHE_SUBGAMES
		cache_join_store(g1->get()->formula(), my_alpha.get(), g1, g2, sub);
#endif
	    }
	    DEBUG(tab_prefix(level()) << TOSTRING(oldmove) << " x "
		    << TOSTRING(oldmove) << " --> "
		    << sub->get()->toString() << std::endl);
	    const MCGame_f* tmpgame = returngame->add_subgame(oldmove, sub);
	    if (tmpgame != NULL) return tmpgame;
	}
    }
    return returngame->minimize();
}

template <typename TFormula, typename TMove, MCGame::Player _player>
const MCGame_f* QUndetGame<TFormula, TMove, _player>::join_impl(const MCGame_f* other,
							   const Assignment_f* alpha,
							   tag<PointMove>) const {
    typedef QUndetGame<TFormula, TMove, _player> MyType;
    typedef PairIterator<subgames_iterator, subgames_iterator> GamePairIterator;
    typedef typename GamePairIterator::value_type GamePairIteratorValue;
    const MyType* gother = static_cast<const MyType*>(other->get());
    MyType* returngame = new MyType(formula());
    DEBUG(returngame->level(level()));

    /*
     * We need to combine in a Cartesian manner:
     * - all TERMINAL moves with all TERMINAL moves on the other side (use the same move)
     * - all ANON moves with the NULL move on the other side, and vice versa;
     *	 this case includes the NULL x NULL case
     */
    const MyType* sides[] = { this, gother }; 
    for (int i = 0; i < 2; i++) {
        DEBUG(tab_prefix(level()) << "Join " << sides[i] << " with "
                << sides[1-i] << std::endl);
	const_iterator mit = sides[i]->begin();
	const_iterator mitend = sides[i]->end();
	for (; mit != mitend; mit++) {
            const PointMove* oldmove = mit->first;
            DEBUG(tab_prefix(level()) << "Found Games for: "
                    << TOSTRING(oldmove) << std::endl);
            // in the second run we do not need to re-check Terminal x Terminal
            // or NULL x *
            if (i == 1) {
                if (oldmove == NULL || oldmove->terminal()) {
                    DEBUG(tab_prefix(level()) << " --> checked before, skip" << std::endl);
                    continue;
                }
            }

            // set the lookup key that we expect for this move
	    // for terminals we require the same move on the other side,
	    // in all other cases the NULL move is the correct one.
            const PointMove* lookupmove = NULL;
            if (oldmove != NULL && oldmove->terminal())
                lookupmove = oldmove;

            // now lookup the corresponding SubGamesContainer container; continue if nonexistent
	    const_iterator mit2 = sides[1-i]->find(lookupmove);
	    if (mit2 == sides[1-i]->end()) {
                DEBUG(tab_prefix(level()) << TOSTRING(lookupmove)
                        << " nonexistent in other, "
                        << (i == 1 ? "break" : "skip")
                        << std::endl);
                // In the second visit of the for loop, we already
                // joined all terminals, so we can stop here early (saves a bit of time)
                // In the first run, we just continue with the next move.
                if (i == 1)
                    break;
                else
                    continue;
            }
            
	    boost::scoped_ptr<const Assignment_f> my_alpha(AssignmentFlyFactory::make(
                new ObjAssignment(alpha, formula()->variable(), oldmove)));

	    GamePairIterator git(mit->second->begin(), mit->second->end(),
	                         mit2->second->begin(), mit2->second->end());
	    while (git.has_next()) {
		GamePairIteratorValue res = git.next();
		const MCGame_f* g1 = res.first;
		const MCGame_f* g2 = res.second;
#if USE_CACHE_SUBGAMES
		const MCGame_f *sub = cache_join_lookup(g1->get()->formula(), my_alpha.get(), g1, g2);
#else
		const MCGame_f *sub = NULL;
#endif
		if (sub == NULL) {
		    sub = g1->get()->join(g2, my_alpha.get());
#if USE_CACHE_SUBGAMES
		    cache_join_store(g1->get()->formula(), my_alpha.get(), g1, g2, sub);
#endif
		}
		DEBUG(tab_prefix(level()) << TOSTRING(oldmove) << " x "
                            << TOSTRING(lookupmove) << " --> "
                            << sub->get()->toString() << std::endl);
		const MCGame_f* tmpgame = returngame->add_subgame(oldmove, sub);
		if (tmpgame != NULL) return tmpgame;
	    }
	}
    }
    return returngame->minimize();
}

#ifdef DODEBUG
template <typename TFormula, typename TMove, MCGame::Player _player> 
void QUndetGame<TFormula, TMove, _player>::recursive_print_game(int level,
								bool skip_first) const {
    (skip_first ? std::cout : tab_prefix(level))
	    << "[" << toString() 
	    << " with subgames" << std::endl;

    typename QUndetGame<TFormula, TMove, _player>::GamesContainer::GameIterator
    	it(QUndetGame<TFormula, TMove, _player>::begin(),
	   QUndetGame<TFormula, TMove, _player>::end());
    while (it.has_next()) {
	const std::pair<const TMove*, const MCGame_f*> res = it.next();
	tab_prefix(level+1) << TOSTRING(res.first) << ": " << std::endl;
	res.second->get()->recursive_print_game(level+2, false);
    }
    tab_prefix(level) << "]" << std::endl;
}
#endif

}; // namespace
