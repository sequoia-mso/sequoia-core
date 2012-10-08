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
#ifndef SEQUOIA_PAIR_ITERATOR_H
#define SEQUOIA_PAIR_ITERATOR_H

#include <assert.h>
#include <utility>

namespace sequoia {

/**
 * A Cartesian-product style iterator (Java-style)
 * that, given two iterator-ranges i1,i1end and i2,i2end,
 * returns each pair <i1,i2> of values in the range exactly once.
 */
template <class It1, class It2>
class PairIterator {
public:
#if 0
    typedef std::pair<It1, It1> it1_range_t;
    typedef std::pair<It2, It2> it2_range_t;
    #endif
    typedef typename It1::value_type it1_value_type;
    typedef typename It2::value_type it2_value_type;
    typedef std::pair<it1_value_type, it2_value_type> value_type;
#if 0
    PairIterator<It1, It2>(const it1_range_t& it1_range,
			   const it2_range_t& it2_range) 
    : _it1(it1_range.first), _it1end(it1_range.second),
      _it2(it2_range.first), _it2end(it2_range.second),
      _has_next(false) {
	if (_it2 == _it2end) _it1 = _it1end; // force empty
	reload_it2();
    }
#endif
    PairIterator<It1, It2>(const It1& it1, const It1& it1end,
			   const It2& it2, const It2& it2end)
    : _it1(it1), _it1end(it1end),
      _it2(it2), _it2end(it2end),
      _has_next(false) {
	if (_it2 == _it2end) _it1 = _it1end; // force empty
	reload_it2();
    }
    value_type next() {
	assert(has_next());
	it1_value_type it1_val = *_it1;
	it2_value_type it2_val = *_cit2;
	advance_it2();
        return std::make_pair(it1_val, it2_val);
    }
    bool has_next() const { return _has_next; }
private:
    value_type _next;
    It1 _it1, _it1end;
    It2 _it2, _cit2, _it2end;
    bool _has_next;

    void advance_it2() {
        _has_next = false;
	_cit2++;
        if (_cit2 != _it2end) {
            _has_next = true;
            return;
        }
	_it1++;
        reload_it2();
    }
    void reload_it2() {
	if (_it1 != _it1end) {
	    _cit2 = _it2;
	    if (_cit2 != _it2end)
		_has_next = true;
        }
    }
};

}; // namespace;

#endif // SEQUOIA_PAIR_ITERATOR_H
