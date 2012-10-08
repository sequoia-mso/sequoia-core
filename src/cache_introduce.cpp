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
#include <boost/smart_ptr/scoped_ptr.hpp>

#include "cache.h"
#include "cache_introduce.h"

namespace sequoia {

namespace internal {

typedef CacheApplyGame<CacheIntroduceValue> CacheIntroduce;

CacheIntroduce cache_introduce;

} // namespace internal

/**
 * @see CacheApplyGame
 */
void
cache_introduce_store(const Formula *formula, const Assignment_f *alpha,
                      const MCGame_f *game, const ConstantSymbol *sym,
                      const CacheIntroduceValue *result) {
#if USE_CACHE
    if (formula->height() < CACHE_CUTOFF) return;
    internal::cache_introduce.store(formula, alpha, game, sym, result);
#endif
}

/**
 * @see CacheApplyGame
 */
const CacheIntroduceValue*
cache_introduce_lookup(const Formula *formula, const Assignment_f *alpha,
                       const MCGame_f *game, const ConstantSymbol *sym) {
#if USE_CACHE
    if (formula->height() < CACHE_CUTOFF) return NULL;
    return internal::cache_introduce.lookup(formula, alpha, game, sym);
#else
    return NULL;
#endif
}

void
cache_introduce_resize(size_t cache_size) {
    internal::cache_introduce.resize(cache_size);
}


} // namespace