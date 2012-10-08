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

#ifdef HAVE_TBB
#include <tbb/atomic.h>
#include <tbb/queuing_rw_mutex.h>
#endif

namespace sequoia {

/**
 * An implementation of the flyweight pattern.
 */
template <typename Type> class Flyweight {
private:
    // Storing pairs is much faster than storing pointers to pairs (up to 10% speedup)
#ifdef HAVE_TBB
    typedef tbb::atomic<size_t> Counter;
#else
    typedef size_t Counter;
#endif
    typedef std::pair<const Type*, Counter*> Entry;
    struct Hasher {
        size_t operator()(const Entry& in) const {
            assert(in.first != NULL);
	    assert(in.first->hash() != 0);
            return in.first->hash();
        }
    };
    struct Equals {
        size_t operator()(const Entry& e1, const Entry& e2) const {
#if 0
            // should not happen
	    if (e1.first == NULL && e2.first == NULL) return true; 
#else
	    assert(!(e1.first == NULL && e2.first == NULL));
#endif
            return *e1.first == *e2.first;
        }
    };
    typedef SEQUOIA_CONCUR_UNORDERED_SET<Entry, Hasher, Equals> Pool;
public:
    typedef const Entry* Handle;
    Flyweight<Type>(const Type* entry) {
        Counter *count = new Counter(); // TBB requires default initialization first
	*count = 1UL;
        Entry p(entry, count);
#ifdef HAVE_TBB
	Mutex::scoped_lock lock(mutex()); // allow multiple "readers".  Only unsafe_erase() must be write-locked
#endif
        std::pair<typename Pool::iterator, bool> res = pool().insert(p);
        _handle = &(*res.first);
        if (!res.second) { // already existing
#ifdef FLYWEIGHT_DEBUG
	    DPRINTLN("[Flyweight<" << typeid(Type).name() << "> new " << entry
		    << "- exists already at " << _handle->first 
		    << " => " << *_handle->second << "]");
#endif
            // increase number of references only
            (*_handle->second)++;
#ifdef HAVE_TBB
	    lock.release();
#endif
            // delete temporary allocated memory
            delete count;
            // free the entry we were supposed to store
            delete entry;
        }
#ifdef FLYWEIGHT_DEBUG
        DPRINTLN("[Flyweight<" << typeid(Type).name() << "> new " << _handle->first
                << " => " << *_handle->second << "]");
#endif
    };
    Flyweight<Type>(const Flyweight& other) {
        // having another instanciated object (other) means there is
	// at least ONE other object in THIS thread referencing 
	// THIS instance.  This means, in any other threads,
	// the coutner is ALWAYS > 0 and we will NOT remove
	// the flyweight from the pool.  We therefore
	// do not need to aquire a global lock, but can rely
	// on the atomic counter to increase this counter.
        _handle = other._handle;
        (*_handle->second)++;
#ifdef FLYWEIGHT_DEBUG
        DPRINTLN("[Flyweight<" << typeid(Type).name() << "> copy "
                << _handle->first << " => " << *_handle->second << "]");
#endif
    }
    ~Flyweight<Type>() {
        assert(*_handle->second > 0);
#ifdef FLYWEIGHT_DEBUG
        DPRINTLN("[Flyweight<" << typeid(Type).name() << "> destroy "
                << _handle->first << " => " << *_handle->second << "]");
#endif
#ifdef HAVE_TBB
	Mutex::scoped_lock lock(mutex(), true); // acquire write lock
#endif
        (*_handle->second)--;
        if (*_handle->second == 0) {
            const Type* e = _handle->first;
            Counter * c = _handle->second;
#ifdef HAVE_TBB
            pool().unsafe_erase(*_handle);
	    lock.release(); // release before deletion
#else
            pool().erase(*_handle);
#endif
            delete e;
            delete c;
        }
    }
    const Type* get() const { return _handle->first; }
    bool operator==(const Flyweight<Type>& other) const {
        return _handle->first == other._handle->first;
    }
    bool operator!=(const Flyweight<Type>& other) const {
        return _handle->first != other._handle->first;
    }
    const Flyweight<Type>* clone() const {
        return new Flyweight(*this);
    }
private:
    Handle _handle;
#ifdef HAVE_TBB
    typedef tbb::queuing_rw_mutex Mutex;
    Mutex& mutex() { return static_holder_class::mutex(); }
#endif
    Pool& pool() { return static_holder_class::get(); }
    // returns singleton instances
    struct static_holder_class {
	static Pool & get() {
	    static Pool instance; 
	    return instance;
	}
#ifdef HAVE_TBB
	static Mutex& mutex() {
	    static Mutex instance;
	    return instance;
	}
#endif
    }; 
};

template <typename T, typename H = hash_func_hasher<T> >
struct flyweight_deep_hasher {
    typedef Flyweight<T> InFly;
    size_t operator()(const InFly& in) const {
        H h;
        return h(*in.get());
    }
};

} // namespace
