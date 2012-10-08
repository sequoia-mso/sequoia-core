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
#ifndef SEQUOIA_CACHE_APPLY_GAME_H
#define	SEQUOIA_CACHE_APPLY_GAME_H

#include "cache.h"
#include "game.h"
#include "logic/assignment.h"
#include "unordered_defs.h"

namespace sequoia { namespace internal {

class CacheApplyGameKey {
public:
    /**
     * No cloning required, handled internally by class.
     */
    CacheApplyGameKey(const Formula *f, const Assignment_f *a, const MCGame_f *g, const ConstantSymbol *s)
    : _formula(f), _alpha(*a), _game(*g), _symbol(s) { } 
    bool operator==(const CacheApplyGameKey &other) const {
	if (_formula != other._formula) return false;
	if (_alpha != other._alpha) return false;
	if (_game != other._game) return false;
	return _symbol == other._symbol;
    }
    size_t hash() const {
	size_t h = hash_init();
        hash_combine(h, _formula->hash());
        hash_combine(h, _alpha.get()->hash());
	hash_combine(h, _game.get()->hash());
	hash_combine(h, _symbol->hash());
	return h;
    }
private:
    const Formula *_formula;
    const Assignment_f _alpha;
    const MCGame_f _game;
    const ConstantSymbol *_symbol;
};

template <typename Value>
class CacheApplyGame {
    typedef Cache<
        CacheApplyGameKey,
        Value,
        hash_func_hasher<CacheApplyGameKey>
    > CacheImpl;
public:
    /**
     * Stores the entry. Will internally create clones of all parameters.
     * No external cloning required.
     */
    void
    store(const Formula *formula, const Assignment_f *alpha,
          const MCGame_f *game, const ConstantSymbol *sym,
          const Value *result) {
#if USE_CACHE
        const CacheApplyGameKey *key = new CacheApplyGameKey(formula, alpha, game, sym);
	_container.store(key, result->clone());
#endif
    }
    /**
    * Returns an entry or NULL if nonexistent. Entry must be deleted after usage.
    */
    const Value*
    lookup(const Formula *formula, const Assignment_f *alpha, const MCGame_f *game,
           const ConstantSymbol *sym) {
#if USE_CACHE
        CacheApplyGameKey key(formula, alpha, game, sym);
	return _container.lookup(key);
#else
        return 0;
#endif
    }

    void resize(size_t size) {
        _container.resize(size);
    }
private:
    CacheImpl _container;
};

} } // namespace

#endif	/* SEQUOIA_CACHE_APPLY_GAME_H */
