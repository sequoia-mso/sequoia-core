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
#include "apply_game_forget.h"
#include "cache_forget.h"
#include "moves_pool.h"
#include "temp_symbol_factory.h"

#include <boost/scoped_ptr.hpp>

namespace sequoia {

template <typename Solver>
void
ApplyGameForget<Solver>::init() {
    boost::scoped_ptr<const Assignment_f> alpha(_solver->alpha(_node)->clone());
    const std::vector<const UnarySymbol*> &free_vars = _solver->free_unary_symbols();

    boost::scoped_ptr<PointMove> tmp_replacement(new PointMove(*_game_info.replacement));

    // for each free set variable, update the SetMove
    for (unsigned int i = 0; i < free_vars.size(); i++) {
	const UnarySymbol *sym = free_vars[i];
	const SetMove* oldsmove = _child_alpha->get()->get(sym);
	SetMove* tmpsmove = new SetMove(*oldsmove);
	tmpsmove->forget(_forgotten_ts, _solver->signature_depth());
	const SetMove* newsmove = moves_pool.pool(tmpsmove);
	DPRINTLN("Var: " << sym->identifier() << ":: " << std::endl
		<< oldsmove->toString()
		<< " --> " << std::endl
		<< newsmove->toString());
	alpha.reset(AssignmentFlyFactory::make(
	    new SetAssignment(alpha.get(), sym, newsmove)));
        const ConstantSymbol *tmp_symbol = create_temporary_symbol(sym->nesting_depth() + 1);
        assert(_forgotten_ts != NULL);
        assert(tmp_symbol != NULL);
        assert(sym != NULL);
        tmp_replacement.reset(tmp_replacement->rename_introduce(sym, tmp_symbol, oldsmove->test(_forgotten_ts)));
    }
    _replacement = moves_pool.pool(new PointMove(*tmp_replacement));
    _alpha = alpha->clone();
}

template <typename Solver>
const MCGame_f *
ApplyGameForget<Solver>::result(const MCGame_f* oldgame) {
    /* 
     * The new alpha already contains the node and its full adjacency
     * (via the terminal moves).  Use this to look up an entry in the cache.
     */
    const MCGame_f *cached = cache_forget_lookup(oldgame->get()->formula(),
                                                 _alpha, oldgame, _forgotten_ts);
    if (cached != NULL) {
	DPRINTLN("Forget Cache hit");
	DPRINTLN("Cache result: " << cached->get()->toString());
	return cached;
    }

    const MCGame_f* newgame = oldgame->get()->forget(_forgotten_ts,
						     _solver->signature_depth(),
                                                     _replacement,
						     _alpha);
    DPRINTLN("Result:");
    DEBUG(newgame->get()->recursive_print_game(0, true));
    DPRINTLN("");
    cache_forget_store(oldgame->get()->formula(), _alpha, oldgame,
                       _forgotten_ts, newgame);
    return newgame;
}

template <typename Solver>
void
ApplyGameForget<Solver>::operator()(const GameVoidPtrMap::value_type &entry) {
    const MCGame_f *oldgame = entry.first;
    const void *oldvalue = entry.second;

    DPRINTLN("-------------------------------------------------------");
    DPRINT("Next Input: Game" << oldgame->get()->toString());
    DPRINTLN("\twith value: " << _solver->evaluation()->toString(oldvalue));

    const MCGame_f *res = result(oldgame);
    _solver->table(_node)->update_value(_alpha, res, oldvalue);
    _count++;
}

} // namespace