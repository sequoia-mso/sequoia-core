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
#ifndef SEQUOIA_GAME_MAP_H
#define SEQUOIA_GAME_MAP_H

#include "game.h"
#include "game_set.h"
#include "unordered_defs.h"

namespace sequoia {

/**
 * Provides a container that for each TMove flyweight (template
 * parameter TMove expected to be one of SetMove/PointMove) holds
 * a set of MCGame flyweights.
 * 
 * Does NOT clone the flyweights upon insertion for efficiency reasons (new
 * is too expensive).  Instead, it takes care of destruction of
 * this flyweight (either on insertion [duplicate entries] or on destruction).
 * Make sure the flyweight pointer inserted is not deleted elsewhere!
 */
template <typename Key, typename Hasher, typename Equals = std::equal_to<Key> >
class GameMap {
    typedef SEQUOIA_UNORDERED_MAP<
        Key,
	MCGameSet *,
	Hasher,
	Equals
    > Container;
    typedef MCGameSet SubgamesContainer;
public:
    typedef typename Container::const_iterator const_iterator;
    typedef typename SubgamesContainer::const_iterator const_subgames_iterator;
    const_iterator find(const Key& key) const { return _container.find(key); }
    const_iterator begin() const { return _container.begin(); }
    const_iterator end() const { return _container.end(); }
    const_subgames_iterator games_begin(const const_iterator &it) const {
        assert(it->second != NULL);
        return it->second->begin();
    }
    const_subgames_iterator games_end(const const_iterator &it) const {
        assert(it->second != NULL);
        return it->second->end();
    }
    size_t size() const { return _container.size(); }

    GameMap() : _hash(0UL) { }
    ~GameMap() {
        const_iterator it = begin(), itend = end();
	for (; it != itend; it++) delete it->second;
    }
    bool operator==(const GameMap& other) const {
	if (this->size() != other.size()) return false;
	if (this->hash() != other.hash()) return false;
	const_iterator it = _container.begin();
	const_iterator itend = _container.end();
	const_iterator otherend = other._container.end();
	for (; it != itend; it++) {
	    const_iterator oit = other._container.find(it->first);
	    if (oit == otherend) return false;
	    assert(it->second != NULL);
	    assert(oit->second != NULL);
	    if (*it->second != *oit->second) return false;
	}
	return true;
    }
    bool insert(const Key& key, const MCGame_f* game) {
	typename Container::const_iterator it = _container.find(key);
	SubgamesContainer *set = NULL;
    	if (it == _container.end()) {
	    set = new SubgamesContainer();
	    _container[key] = set;
	} else {
	    set = it->second;
	}
	bool isnew = set->insert(game);
	return isnew;
    }
	
private:
    size_t hash() const {
        if (_hash == 0UL)
	    (const_cast<GameMap *>(this))->compute_hash();
        return _hash; 
    }
    void compute_hash() {
        size_t h = hash_init();
	const_iterator it = _container.begin();
	const_iterator itend = _container.end();
	for (; it != itend; it++) {
	    if (it->first != NULL)
	        hash_combine(h, it->first->hash());
	    else
	        hash_combine(h, 13);
	    hash_combine(h, it->second->hash());
	}
	_hash = h;
    }

    Container _container;
    /* forbid these */
    GameMap(const GameMap& other) { };
    GameMap& operator=(const GameMap& other) { return *this; };
    size_t _hash;

public:
    /**
     * An iterator that returns the games in order
     */
    class GameIterator {
    public:
    	typedef typename std::pair<const Key, const MCGame_f*> value_type;
	GameIterator(const const_iterator &mit, const const_iterator &mitend)
	: _mit(mit), _mitend(mitend), _has_next(false) {
	    init_keys_it();
	}
	value_type next() {
	    assert(_has_next);
	    const MCGame_f* next_game = *_git;
	    const Key &cur_move = _mit->first;
	    advance_game_it();
	    return std::make_pair(cur_move, next_game);
	}
	bool has_next() const { return _has_next; }
    private:
	const_iterator _mit, _mitend;
	bool _has_next;
	const_subgames_iterator _git, _gitend;
	void advance_game_it() {
	    _has_next = false;
	    _git++;
	    if (_git != _gitend) {
		_has_next = true;
		return;
	    }
	    _mit++;
	    init_keys_it();
	}
	void init_keys_it() {
	    if (_mit != _mitend) {
	        _git = _mit->second->begin();
		_gitend = _mit->second->end();
		if (_git != _gitend)
		    _has_next = true;
	    }
	}
    };
};

}; // namespace

#endif // SEQUOIA_GAME_MAP_H
