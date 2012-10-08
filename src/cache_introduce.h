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
#ifndef SEQUOIA_CACHE_INTRODUCE_H
#define	SEQUOIA_CACHE_INTRODUCE_H

#include "cache_apply_game.h"
#include "game.h"
#include "logic/assignment.h"

namespace sequoia {

class CacheIntroduceValue {
public:
    typedef std::pair<const Assignment_f*, const MCGame_f*> CacheIntroduceValueItem;
    typedef std::vector<CacheIntroduceValueItem> Container;
    typedef Container::const_iterator const_iterator;
    CacheIntroduceValue() : _container() { }
    explicit CacheIntroduceValue(const CacheIntroduceValue &other) : _container() {
	const_iterator it;
	for (it = other._container.begin(); it != other._container.end(); it++) {
	    _container.push_back(std::make_pair(it->first->clone(),
					     it->second->clone()));
	}
    }
    const CacheIntroduceValue *clone() const { return new CacheIntroduceValue(*this); }
    ~CacheIntroduceValue() {
	const_iterator it;
	for (it = _container.begin(); it != _container.end(); it++) {
	    delete it->first;
	    delete it->second;
	}
    }
    const_iterator begin() const { return _container.begin(); }
    const_iterator end() const { return _container.end(); }
    void add(const Assignment_f *a, const MCGame_f *g) {
	_container.push_back(std::make_pair(a, g));
    }
private:
    Container _container;
};

extern void
cache_introduce_store(const Formula *formula, const Assignment_f *alpha,
                      const MCGame_f *game, const ConstantSymbol *sym,
                      const CacheIntroduceValue *result);

extern const CacheIntroduceValue*
cache_introduce_lookup(const Formula *formula, const Assignment_f *alpha,
                       const MCGame_f *game, const ConstantSymbol *sym);

extern void
cache_introduce_resize(size_t cache_size);

} // namespace

#endif	// SEQUOIA_CACHE_INTRODUCE_H 

