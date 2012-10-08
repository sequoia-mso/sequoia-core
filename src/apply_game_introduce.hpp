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
#include "apply_game_introduce.h"
#include "cache_introduce.h"
#include "moves_pool.h"

#include <boost/scoped_ptr.hpp>

namespace sequoia {

template <typename Solver>
void
ApplyGameIntroduce<Solver>::init() {
}

/*
 * Compute the intersection for the evaluation.
 */
template <typename Solver>
const Assignment_f *
ApplyGameIntroduce<Solver>::compute_intersection(const Assignment_f *alpha) const {
    boost::scoped_ptr<const Assignment_f> result(AssignmentFlyFactory::make(new EmptyAssignment()));
    // we just need to remove the membership flag for all free variables
    const std::vector<const UnarySymbol*> &free_vars = _solver->free_unary_symbols();
    for (unsigned int i = 0; i < free_vars.size(); i++) {
	const UnarySymbol *sym = free_vars[i];
	const SetMove* smove = alpha->get()->get(sym);
	SetMove* tmpsmove = new SetMove(*smove);
	if (tmpsmove->test(_intro_ts))
	    tmpsmove->remove(_intro_ts);
	const SetMove* newsmove = moves_pool.pool(tmpsmove);
	result.reset(AssignmentFlyFactory::make(
	    new SetAssignment(result.get(), sym, newsmove)));
    }
    return result->clone();
}

/**
 * save_value takes care of deleting the game flyweight, do not delete elsewhere!
 */
template <typename Solver>
void
ApplyGameIntroduce<Solver>::save_value(const Assignment_f* alpha,
				       const MCGame_f* game,
				       const void* oldvalue) const {
    assert(game->get()->outcome() != MCGame::FALSIFIER);
    boost::scoped_ptr<const Assignment_f> intersection(compute_intersection(alpha));
    const void *intersec_elem = _solver->evaluation()->elem(_bag, intersection.get()->get());
    const void *new_elem = _solver->evaluation()->elem(_bag, alpha->get());
    const void *val = _solver->evaluation()->mult(oldvalue, new_elem, intersec_elem);
    _solver->table(_node)->update_value(alpha, game, val);
}

template <typename Solver>
const CacheIntroduceValue*
ApplyGameIntroduce<Solver>::results(const MCGame_f* oldgame) {
    /* 
     * The new alpha already contains the node and its full adjacency
     * (via the terminal moves).  Use this to look up an entry in the cache.
     */
    const Assignment_f* alpha = _solver->alpha(_node);
    const CacheIntroduceValue* cached = cache_introduce_lookup(oldgame->get()->formula(),
                                                               alpha, oldgame, _intro_ts);
    if (cached != NULL) {
	DPRINTLN("Introduce Cache hit");
	typename CacheIntroduceValue::const_iterator it;
	DEBUG({
	    for (it = cached->begin(); it != cached->end(); it++) {
		const MCGame_f *cached_game = it->second;
		DPRINTLN("Cache result: " << cached_game->get()->toString());
	    }
	});
	return cached;
    }

    /*
     * Otherwise, prepare an entry: For each of the 2^{|free_vars|} ways
     * to introduce the new terminal, create a new table entry.
     */
    CacheIntroduceValue *res = new CacheIntroduceValue();
    const std::vector<const UnarySymbol*> &free_vars = _solver->free_unary_symbols();
    size_t stop = 1UL << free_vars.size();

    for (size_t setbits = 0UL; setbits < stop; setbits++) {
	boost::scoped_ptr<const Assignment_f> my_alpha(alpha->clone());
	for (unsigned int i = 0; i < free_vars.size(); i++) {
	    bool member = (0x1UL & (setbits >> i));
	    const UnarySymbol *sym = free_vars[i];
	    const SetMove* oldsmove = _child_alpha->get()->get(sym);
	    SetMove* tmpsmove = new SetMove(*oldsmove);
	    tmpsmove->introduce(_intro_ts, member, _solver->signature_depth());
	    DPRINT("Var: " << sym->identifier() << ", member: " << (member ? "yes" : "no") << ":\t");
	    const SetMove* newsmove = moves_pool.pool(tmpsmove);
	    DPRINTLN(oldsmove->toString() << " --> " << newsmove->toString());
	    assert(!my_alpha->get()->assigned(sym));
	    my_alpha.reset(AssignmentFlyFactory::make(
		new SetAssignment(my_alpha.get(), sym, newsmove)));
	    assert(my_alpha->get()->assigned(sym));
	}
	const MCGame_f *newgame = oldgame->get()->introduce(_intro_ts,
							    _solver->signature_depth(),
							    my_alpha.get());
    	DPRINTLN("Result:");
    	DEBUG(newgame->get()->recursive_print_game(0, true));
    	DPRINTLN("");
	if (newgame->get()->outcome() != MCGame::FALSIFIER) {
	    // Clone my_alpha, because my_alpha is a scoped_ptr.
	    // Do not clone the game since this is the desired storage
	    // location.  Both pointers will be freed when reslist is
	    // destroyed.
	    res->add(my_alpha->clone(), newgame);
	}
    }
    cache_introduce_store(oldgame->get()->formula(),
                          alpha, oldgame, _intro_ts, res);
    return res;
}

template <typename Solver>
void
ApplyGameIntroduce<Solver>::operator()(const GameVoidPtrMap::value_type &entry) {
    const MCGame_f *oldgame = entry.first;
    const void *oldvalue = entry.second;

    DPRINTLN("-------------------------------------------------------");
    DPRINT("Next Input: Game" << oldgame->get()->toString());
    DPRINTLN("\twith value: " << _solver->evaluation()->toString(oldvalue));
    DEBUG(oldgame->get()->recursive_print_game(0, true));
    DPRINTLN("");

    const CacheIntroduceValue *reslist = results(oldgame);
    typename CacheIntroduceValue::const_iterator it;
    for (it = reslist->begin(); it != reslist->end(); it++) {
	save_value(it->first, it->second->clone(), oldvalue);
        _count++;
    }
    delete reslist;
}

} // namespace