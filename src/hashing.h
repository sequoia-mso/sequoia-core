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
#ifndef SEQUOIA_HASHING_H
#define SEQUOIA_HASHING_H

#include <assert.h>
#include <cstring>	 // for size_t
#include <functional>    // for equal_to
#include <memory>        // for alloc<>
#include <utility>       // for pair<>

#include <boost/functional/hash.hpp>

namespace sequoia {

#define SEQUOIA_BOOST_HASHING 1
inline size_t hash_init() {
#if SEQUOIA_BOOST_HASHING && 0
  return 0UL;
#else
  return 5381;
#endif
}
inline void hash_combine(size_t& seed, const size_t& h) {
#if SEQUOIA_BOOST_HASHING
    boost::hash_combine(seed, h);
#else
    seed = ((seed << 5) + seed) + (h+1);
#endif
}
inline size_t hash_value(const char* c) {
#if SEQUOIA_BOOST_HASHING
    return boost::hash_value(c);
#else
    size_t h = hash_init();
    while (*c != '\0')
        hash_combine(h, *c++);
    return h;
#endif
}

template <typename T>
struct hash_func_hasher {
    size_t operator()(const T& in) const {
        return in.hash();
    }
};
/**
 * Hash by the object a pointer references
 */
template <typename T, typename H = hash_func_hasher<T> >
struct ptr_deep_hasher {
    size_t operator()(const T* in) const {
        if(in == NULL) return 1UL;
        H h;
        return h(*in);
    }
};

/**
 * Compare two pointers by object a pointer references
 */
template <typename T > struct ptr_deep_equals {
    size_t operator()(const T& p1, const T& p2) const {
        if (p1 == p2) return true;
        if (p1 == NULL || p2 == NULL) return false;
	return *p1 == *p2;
    }
};

} // namespace

#endif
