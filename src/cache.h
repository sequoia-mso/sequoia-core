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
#ifndef SEQUOIA_CACHE_H
#define SEQUOIA_CACHE_H

#include "common.h"
#include "hashing.h"
#include "list.h"
#include "unordered_defs.h"

#include <unordered_map>

#ifdef HAVE_TBB
#include <tbb/enumerable_thread_specific.h>
#endif

namespace sequoia {

namespace internal {
/**
 * A LRU Cache class for lookups of type Key to Values of type Value.
 * Key and Value are the plain object types, however we only handle
 * pointers to the respective objects here.  
 * Value class must be clone()-able.
 */
template <typename Key, typename Value,
        typename Hash, typename Equals = std::equal_to<Key> >
class CacheImpl {
public:
    CacheImpl(const size_t *max_size) : _max_size(max_size), _size(0UL) { }
    // do not create a full copy, instead create a new empty cache
    CacheImpl(const CacheImpl &other)
    : _list(), _cache(), _max_size(other._max_size), _size(0UL) { }
    typedef Value value_type;
    typedef typename std::pair<const Key*, const Value*> KeyValuePair;

    /**
     * Lookup the value stored for the given key.
     * Returns a cloned() instance.
     * @return the value found for key, or NULL if non-existent
     */
    const Value* lookup(const Key* key) {
#if USE_CACHE
        //DPRINTLN("[Cache<" << typeid(K).name() << "> lookup: " << key);
        typename ContainerImpl::const_iterator cit = _cache.find(key);
        if (cit == _cache.end()) {
            //DPRINTLN("lookup failed");
            return NULL;
        }
        //DPRINTLN("lookup sucessful: " << cit->second);
        const ListHandle &it = cit->second;
	// move element to end of list (LRU)
        _list.move_back(it);
        const KeyValuePair *entry = it->value();
	return entry->second->clone();
#else
        return NULL;
#endif
    }
    const Value* lookup(const Key& key) { return lookup(&key); }
    /**
     * Try to store the value value at position key. 
     * @param key the key for the store
     * @param value the value to store
     * @return true iff the entry was new
     */
    bool store(const Key* key, const Value* value) {
#if USE_CACHE
	if (*_max_size == 0) {
	    delete key;
	    delete value;
	    return false;
	}
        bool res = this->store_impl(key, value);
	maybe_purge();
	return res;
#else
	return false;
#endif
    }

private:

    bool store_impl(const Key* key, const Value* value) {
#if USE_CACHE
        //DPRINTLN("[Cache<" << typeid(K).name() << "> store: " << key << " => " << value);
        // first insert the entry to the list to obtain a handle
        const KeyValuePair *entry = new KeyValuePair(key, value);
        ListHandle it = _list.push_back(entry);
        // now insert the key and the iterator into the hash map
        CacheValueType hashentry(key, it);
        std::pair<CacheIterator, bool> res = _cache.insert(hashentry);
        if (res.second) { // new entry
            _size++;
	    return true;
        }
        // otherwise, we need to remove the duplicated entry from the
	// list and free the associated pointers
        _list.erase(it);
        delete entry->first;
        delete entry->second;
	delete entry;
#endif // USE_CACHE
        return false;
    }

    void maybe_purge() {
        if (_size <= *_max_size) return;
	size_t topurge = _size - *_max_size;
	for (unsigned int i = 0; i < topurge; i++) {
            const KeyValuePair* kvp = _list.pop_front();
            _cache.erase(kvp->first);
	    delete kvp->first;
	    delete kvp->second;
	    delete kvp;
	    _size--;
	}
	assert(_size <= *_max_size); // catch bug if _purge_size = 0
    }

    typedef List<KeyValuePair> ListImpl;
    typedef typename ListImpl::Handle ListHandle;
    typedef typename std::unordered_map<
	const Key*,
	ListHandle,
	ptr_deep_hasher<Key, Hash>,
	ptr_deep_equals<const Key *>
    > ContainerImpl;
    typedef typename ContainerImpl::value_type CacheValueType;
    typedef typename ContainerImpl::iterator CacheIterator;

    ListImpl _list;
    ContainerImpl _cache;
    size_t _size;
    const size_t *_max_size;
};

} // internal

template <typename Key, typename Value,
          typename Hash, typename Equals = std::equal_to<Key> >
class Cache {
    typedef internal::CacheImpl<Key, Value, Hash, Equals> CacheImpl;
#ifdef HAVE_TBB
    typedef tbb::enumerable_thread_specific<CacheImpl> ImplWrapper;
#endif
public:
    Cache<Key, Value, Hash, Equals>()
    : _max_size(CACHE_DEFAULT_SIZE),
#ifdef HAVE_TBB
      _impl(CacheImpl(&_max_size))
#else
      _impl(&_max_size)
#endif
    { }
    const Value* lookup(const Key* key) {
#ifdef HAVE_TBB
        typename ImplWrapper::reference mine = _impl.local();
	return mine.lookup(key);
#else
	return _impl.lookup(key);
#endif
    }
    const Value* lookup(const Key &key) {
	return lookup(&key);
    }
    void store(const Key* key, const Value *value) {
#ifdef HAVE_TBB
        typename ImplWrapper::reference mine = _impl.local();
	mine.store(key, value);
#else
        _impl.store(key, value);
#endif
    }
    void resize(size_t size) {
        _max_size = size;
    }
private:
    size_t _max_size;
#ifdef HAVE_TBB
    ImplWrapper _impl;
#else
    CacheImpl _impl;
#endif
};

} // namespace

#endif // SEQUOIA_CACHE_H
