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
#ifndef SEQUOIA_GAME_SET_H
#define SEQUOIA_GAME_SET_H

#include "game.h"
#include "unordered_defs.h"

namespace sequoia {

/**
 * Provides a set container for pointer to MCGames flyweights.
 * Does NOT clone the flyweight upon insertion for efficiency reasons (new
 * is too expensive).  Instead, it takes care of destruction of
 * this flyweight (either on insertion [duplicate entries] or on destruction).
 * Make sure the flyweight pointer inserted is not deleted elsewhere!
 */
class MCGameSet {
public:
    MCGameSet() : _hash(0UL) { }
    ~MCGameSet() {
        const_iterator it = begin();
        const_iterator itend = end();
        for (; it != itend; it++)
            delete *it;
    }
    bool insert(const MCGame_f* game) {
        assert(game != NULL);
        std::pair<Container::iterator, bool> res = _container.insert(game);
        return res.second;
    }
    size_t hash() const {
    if (_hash == 0UL)
	(const_cast<MCGameSet *>(this))->compute_hash();
        return _hash;
    }

    bool operator==(const MCGameSet& other) const {
        if (hash() != other.hash()) return false;
	if (_container.size() != other._container.size()) return false;
	// WARNING!!! Do not use _container == _other.container!
	// This is not necessarily true, since operator== on the values
	// is used to decide whether two sets are a permutation of each other,
	// which is broken for our flyweight types.  We do the correct
	// check ourselves.
	const_iterator it = begin();
	const_iterator itend = end();
	const_iterator oend = other.end();
	for (; it != itend; it++)
	    if (other._container.find(*it) == oend) return false;
	return true;
    }
    bool operator!=(const MCGameSet& other) const {
        return !(*this == other);
    }
    typedef SEQUOIA_UNORDERED_SET<const MCGame_f*,
        ptr_deep_hasher<MCGame_f, flyweight_inheritance_deep_hasher<MCGame> >,
        ptr_deep_equals<const MCGame_f*> > Container;
    typedef Container::const_iterator const_iterator;
    const_iterator begin() const { return _container.begin(); }
    const_iterator end() const { return _container.end(); }
    bool empty() const { return _container.empty(); }
private:
    void compute_hash() {
        size_t h = hash_init();
        const_iterator it = begin();
        const_iterator itend = end();
        for (; it != itend; it++)
	    hash_combine(h, (*it)->get()->hash());
	_hash = h;
    }
    size_t _hash;
    Container _container;
};

}; // namespace

#endif // SEQUOIA_GAME_SET_H
