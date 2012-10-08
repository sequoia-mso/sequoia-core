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
#include "cache_introduce.h"
#include "cache_join.h"
#include "game_bool_comb.h"
#include "game_determined.h"

#include <assert.h>

namespace sequoia {

template <typename TFormula, MCGame::Player _player>
const MCGame_f* BoolCombUndetGame<TFormula, _player>::minimize() const {
    assert(outcome() == MCGame::UNDETERMINED);
    if (empty()) {
    	delete this;
    	return determined.get(MCGame::opponent<_player>::value);
    } else {
        return MCGameFlyFactory::make(this);
    }
}

template <typename TFormula, MCGame::Player _player>
const MCGame_f* BoolCombUndetGame<TFormula, _player>::add_subgame(const Formula* formula,
								  const MCGame_f* game) {
    if (game->get()->outcome() == _player) {
	DPRINTLN(" --> determines!");
	delete this;
	return game;
    } 
    if (game->get()->outcome() == MCGame::opponent<_player>::value) {
	DPRINTLN(" --> ignore!");
	delete game;
	return NULL;
    }
    DPRINTLN(" --> re-check later!");
    set_subgame(formula, game);
    return NULL;
}

template <typename TFormula, MCGame::Player _player>
const MCGame_f* BoolCombUndetGame<TFormula, _player>::convert() const {
    DEBUG({
        tab_prefix(level()) << "Building for game: "
                << toString() << std::endl;
    });
    const TFormula* f = BoolCombUndetGame<TFormula, _player>::formula();
    BoolCombUndetGame<TFormula, _player>* returngame = new BoolCombUndetGame<TFormula, _player>(f);

    subgames_iterator it, itend;
    for (boost::tie(it, itend) = subgames(); it != itend; it++) {
        const Formula* subf = it->first;
        const MCGame_f* subg = it->second;
        const MCGame_f* sub = subg->get()->convert();
        DEBUG(tab_prefix(level()) << "(" << subf->toString()
                << ") --> " << sub->get()->toString());
        const MCGame_f* tmpgame = returngame->add_subgame(subf, sub);
        if (tmpgame != NULL) return tmpgame;
    }
    return returngame->minimize();
}

template <typename TFormula, MCGame::Player _player>
const MCGame_f* BoolCombUndetGame<TFormula, _player>::forget(const ConstantSymbol* tsym,
							     int signature_depth,
							     const PointMove* replacement,
							     const Assignment_f* alpha) const {
    DEBUG({
        tab_prefix(level()) << "Building for game: " << toString() << std::endl;
    });

    const TFormula* f = BoolCombUndetGame<TFormula, _player>::formula();
    BoolCombUndetGame<TFormula, _player>* returngame = new BoolCombUndetGame<TFormula, _player>(f);
    DEBUG(returngame->level(level()));

    subgames_iterator it, itend;
    for (boost::tie(it, itend) = subgames(); it != itend; it++) {
        const Formula *subf = it->first;
        const MCGame_f* subg = it->second;
        DEBUG(tab_prefix(level()) << "(" << subf->toString() << "):" << std::endl);

#if USE_CACHE_SUBGAMES
        const MCGame_f* sub = cache_forget_lookup(subf, alpha, subg, tsym);
#else
	const MCGame_f *sub = NULL;
#endif
	if (sub == NULL) {
            sub = subg->get()->forget(tsym, signature_depth, replacement, alpha);
#if USE_CACHE_SUBGAMES
	    cache_forget_store(subf, alpha, subg, tsym, sub);
#endif
        }
        DEBUG(tab_prefix(level()) << "(" << subf->toString()
                << ") --> " << sub->get()->toString());
        const MCGame_f* tmpgame = returngame->add_subgame(subf, sub);
        if (tmpgame != NULL) return tmpgame;
    }
    return returngame->minimize();
}

template <typename TFormula, MCGame::Player _player>
const MCGame_f* BoolCombUndetGame<TFormula, _player>::introduce(const ConstantSymbol* tsym,
								int signature_depth,
								const Assignment_f* alpha) const {
    DEBUG({
        tab_prefix(level()) << "Building for game: " << toString() << std::endl;
    });
    const TFormula* f = BoolCombUndetGame<TFormula, _player>::formula();
    BoolCombUndetGame<TFormula, _player>* returngame = new BoolCombUndetGame<TFormula, _player>(f);
    DEBUG(returngame->level(level()));

    subgames_iterator it, itend;
    for (boost::tie(it, itend) = subgames(); it != itend; it++) {
        const Formula *subf = it->first;
        const MCGame_f* subg = it->second;
        DEBUG(tab_prefix(level()) << "(" << subf->toString() << "):" << std::endl);

#if USE_CACHE_SUBGAMES
        const CacheIntroduceValue* cached = cache_introduce_lookup(subf, alpha, subg, tsym);
        if (cached == NULL) {
            CacheIntroduceValue *res = new CacheIntroduceValue();
            const MCGame_f *sub = subg->get()->introduce(tsym, signature_depth, alpha);
	    res->add(alpha->clone(), sub);
            cache_introduce_store(subf, alpha, subg, tsym, res);
            cached = res;
        }
        typename CacheIntroduceValue::const_iterator it = cached->begin();
        const MCGame_f *sub = it->second->clone();
        delete cached;
#else
        const MCGame_f *sub = subg->get()->introduce(tsym, signature_depth, alpha);
#endif
        DEBUG(tab_prefix(level()) << "(" << subf->toString()
                << ") --> " << sub->get()->toString());
        const MCGame_f* tmpgame = returngame->add_subgame(subf, sub);
        if (tmpgame != NULL) return tmpgame;
    }
    return returngame->minimize();
}


template <typename TFormula, MCGame::Player _player>
const MCGame_f* BoolCombUndetGame<TFormula, _player>::join(const MCGame_f* other,
							   const Assignment_f* alpha) const {
    DEBUG({
        tab_prefix(level()) << "Joining for game: " << toString() << std::endl;
    });
    if (other->get()->outcome() != MCGame::UNDETERMINED) {
        DEBUG(tab_prefix(level()) << "Found determined game, return." << std::endl);
        return other->clone();
    }
    const TFormula* f = BoolCombUndetGame<TFormula, _player>::formula();
    BoolCombUndetGame<TFormula, _player>* returngame = new BoolCombUndetGame<TFormula, _player>(f);
    DEBUG(returngame->level(level()));

    const BoolCombUndetGame<TFormula, _player>* bother =
	static_cast<const BoolCombUndetGame<TFormula, _player>*>(other->get());

    subgames_iterator it, itend;
    for (boost::tie(it, itend) = subgames(); it != itend; it++) {
        const Formula *subf = it->first;
        const MCGame_f* subg = it->second;
	const MCGame_f* subother = bother->subgame(subf);
	if (subother == NULL) {
            DEBUG(tab_prefix(level()) << "(" << TOSTRING(subf)
                    << ") --> ignored in other, skip" << std::endl);
            continue;
        }
        DEBUG(tab_prefix(level()) << "(" << subf->toString() << "):" << std::endl);
	
#if USE_CACHE_SUBGAMES
	const MCGame_f *sub = cache_join_lookup(subf, alpha, subg, subother);
#else
	const MCGame_f *sub = NULL;
#endif
	if (sub == NULL) {
	    sub = subg->get()->join(subother, alpha);
#if USE_CACHE_SUBGAMES
	    cache_join_store(subf, alpha, subg, subother, sub);
#endif
	}
        DEBUG(tab_prefix(level()) << "(" << subf->toString()
                << ") --> " << sub->get()->toString());
        const MCGame_f* tmpgame = returngame->add_subgame(subf, sub);
        if (tmpgame != NULL) return tmpgame;
    }
    return returngame->minimize();
}

#ifdef DODEBUG
template <typename TFormula, MCGame::Player _player> 
void BoolCombUndetGame<TFormula, _player>::recursive_print_game(int level,
								bool skip_first) const {
    (skip_first ? std::cout : tab_prefix(level))
	    << "[" << toString() << " with subgames" << std::endl;
    subgames_iterator it, itend;
    for (boost::tie(it, itend) = subgames(); it != itend; it++) {
	tab_prefix(level+1) << it->first->toString() << " -> ";
	it->second->get()->recursive_print_game(level+1, true);
    }
    tab_prefix(level) << "]" << std::endl;
}
#endif

} // namespace
