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
#ifndef SEQUOIA_UNORDERED_DEFS_H
#define	SEQUOIA_UNORDERED_DEFS_H

#include "common.h"

// Standard containers
#include <sparsehash/sparse_hash_set>
#include <sparsehash/sparse_hash_map>
#define SEQUOIA_UNORDERED_SET google::sparse_hash_set
#define SEQUOIA_UNORDERED_MAP google::sparse_hash_map

// Thread safe containers when built with multi-threading support
#ifdef HAVE_TBB
#include <tbb/concurrent_unordered_set.h>
#include <tbb/concurrent_unordered_map.h>
#define SEQUOIA_CONCUR_UNORDERED_SET tbb::concurrent_unordered_set
#define SEQUOIA_CONCUR_UNORDERED_MAP tbb::concurrent_unordered_map
#else // HAVE_TBB

#if defined(__APPLE__)
// std::tr1::unordered_* included in MacOS X's ancient g++ version causes crashes
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#define SEQUOIA_CONCUR_UNORDERED_SET boost::unordered_set
#define SEQUOIA_CONCUR_UNORDERED_MAP boost::unordered_map
#else // no MacOS, just use the TR1 version
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#define SEQUOIA_CONCUR_UNORDERED_SET std::tr1::unordered_set
#define SEQUOIA_CONCUR_UNORDERED_MAP std::tr1::unordered_map
#endif // __APPLE__

#endif // HAVE_TBB

#endif	/* SEQUOIA_UNORDERED_DEFS_H */
