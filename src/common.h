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
#ifndef SEQUOIA_COMMON_H
#define	SEQUOIA_COMMON_H

#include "../config.h"

#include <bitset>
#include <climits>  // INT_MIN
#include <iostream>
#include <string>
#include <csignal> // raise() for debugging

#include <boost/version.hpp>

namespace sequoia {

#define USE_CACHE 1
#define USE_CACHE_SUBGAMES 1
#if !USE_CACHE
#warning "Cache is deactivated!"
#endif

// protect users from shooting into their feet: be conservative and
// use relatively low cache values unless they read the documentation
#define CACHE_DEFAULT_SIZE 10000
#define CACHE_CUTOFF    3 // some tests show that this is reasonable

#define TOSTRING(x) ((x) != NULL ? (x)->toString() : "(NULL)")
#ifdef DODEBUG
#define DPRINTLN(x) (std::cout << x << std::endl << std::flush)
#define DPRINT(x)   (std::cout << x << std::flush)
#define DEBUG(x)    x
#else
#define DPRINT(x)
#define DPRINTLN(x)
#define DEBUG(x)
#endif

#ifdef DODEBUG
#define BITSET_SIZE 16
#else
#define BITSET_SIZE (sizeof(unsigned long) * 8)
#endif
typedef std::bitset<BITSET_SIZE> BitSet;

/**
 * Append i tabs ("\t") to std::cout and return it.
 */
std::ostream & tab_prefix(int i);

/**
 * Convert a std::string into a new const char*.
 * This is similar to std::string.c_str() but uses permanent memory.
 * Pointer must manually be deleted after usage.
 */
const char * str2char(std::string s);

} // sequoia

#endif	/* SEQUOIA_COMMON_H */
