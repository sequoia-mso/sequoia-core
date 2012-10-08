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
#include "cache.h"
#include "cache_join.h"
#include "unordered_defs.h"

namespace sequoia {

namespace internal {

class CacheJoinKey {
public:
    /**
     * No cloning required, handled internally by class.
     */
    CacheJoinKey(const Formula *formula,
                 const Assignment_f *alpha,
		 const MCGame_f *game_left,
		 const MCGame_f *game_right)
    : _formula(formula), _alpha(*alpha),
      _game_left(*game_left), _game_right(*game_right) { } 
    bool operator==(const CacheJoinKey &other) const {
	if (_formula != other._formula) return false;
	if (_alpha != other._alpha) return false;
	if (_game_left != other._game_left) return false;
	if (_game_right != other._game_right) return false;
	return true;
    }
    size_t hash() const {
	size_t h = hash_init();
	hash_combine(h, _formula->hash());
	hash_combine(h, _alpha.get()->hash());
	hash_combine(h, _game_left.get()->hash());
	hash_combine(h, _game_right.get()->hash());
	return h;
    }
private:
    const Formula *_formula;
    const Assignment_f _alpha;
    const MCGame_f _game_left;
    const MCGame_f _game_right;
};

class CacheJoin {
    typedef Cache<
        CacheJoinKey,
	CacheJoinValue,
	hash_func_hasher<CacheJoinKey>
    > CacheImpl;
public:
    /**
     * Stores the entry. Will internally create clones of all parameters.
     * No external cloning required.
     */
    void
    store(const Formula *formula, const Assignment_f *alpha,
	  const MCGame_f *game_left, const MCGame_f *game_right, 
	  const CacheJoinValue *result) {
#if USE_CACHE
	const MCGame_f *a = (game_left->get() < game_right->get() ? game_left : game_right);
	const MCGame_f *b = (game_left->get() < game_right->get() ? game_right : game_left);
        const CacheJoinKey *key = new CacheJoinKey(formula, alpha, a, b);
	_container.store(key, result->clone());
#else
        return;
#endif
    }
    /**
     * Returns an entry or NULL if nonexistent. Entry must be deleted after usage.
     */
    const CacheJoinValue*
    lookup(const Formula *formula,
           const Assignment_f *alpha,
	   const MCGame_f *game_left,
	   const MCGame_f *game_right) {
#if USE_CACHE
	const MCGame_f *a = (game_left->get() < game_right->get() ? game_left : game_right);
	const MCGame_f *b = (game_left->get() < game_right->get() ? game_right : game_left);
        CacheJoinKey key(formula, alpha, a, b);
        return _container.lookup(&key);
#else
        return NULL;
#endif
    }

    void resize(size_t size) {
	_container.resize(size);
    }
private:
    CacheImpl _container;
};

CacheJoin cache_join;

} // namespace internal

void
cache_join_store(const Formula *formula,
                 const Assignment_f *alpha,
		 const MCGame_f *game_left,
		 const MCGame_f *game_right,
		 const CacheJoinValue *result) {
#if USE_CACHE
    assert(game_left->get()->formula() == game_right->get()->formula());
    if (formula->height() < CACHE_CUTOFF) return;
    internal::cache_join.store(formula, alpha, game_left, game_right, result);
#else
    return;
#endif
}

const CacheJoinValue*
cache_join_lookup(const Formula *formula,
                  const Assignment_f *alpha,
		  const MCGame_f *game_left,
		  const MCGame_f *game_right) {
#if USE_CACHE
    assert(game_left->get()->formula() == game_right->get()->formula());
    if (formula->height() < CACHE_CUTOFF) return NULL;
    return internal::cache_join.lookup(formula, alpha, game_left, game_right);
#else
    return NULL;
#endif
}

void
cache_join_resize(size_t cache_size) {
    internal::cache_join.resize(cache_size);
}




} // namespace
