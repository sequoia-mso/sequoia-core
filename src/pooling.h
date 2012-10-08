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
#pragma once

#include "common.h"
#include "hashing.h"
#include "unordered_defs.h"

namespace sequoia {

template <typename T> class Pool {
public:
    ~Pool<T>() {
        typename PoolSet::const_iterator cit = _pool.begin();
        typename PoolSet::const_iterator citend = _pool.end();
        for (; cit != citend; cit++)
            delete *cit;
    }
    const T* pooling(const T* inptr) {
        assert(inptr != NULL);
        std::pair<typename PoolSet::iterator, bool> res = _pool.insert(inptr);
	const T* resptr = *res.first;
        if (res.second) { // new
#ifdef POOLING_DEBUG
            DPRINTLN("[Pool<" << typeid(T).name() << "> new " << inptr << "]");
#endif
        } else { // existing
#ifdef POOLING_DEBUG
            DPRINTLN("[Pool<" << typeid(T).name() << "> hit " << inptr << "]");
#endif
	    assert(inptr != resptr);
            delete inptr;
        }
        return resptr;
    }
    size_t size() const { return _pool.size(); }
private:
    typedef SEQUOIA_CONCUR_UNORDERED_SET<
        const T*,
	ptr_deep_hasher<T>,
        ptr_deep_equals<const T*>
    > PoolSet;
    PoolSet _pool;
};

} // namespace
