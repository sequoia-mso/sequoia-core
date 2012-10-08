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
#ifndef SEQUOIA_PAIR_ITERATOR_STL
#define SEQUOIA_PAIR_ITERATOR_STL

#include <boost/iterator/iterator_facade.hpp>

namespace sequoia {

/**
 * A Cartesian-product style iterator
 * that, given two iterator-ranges i1,i1end and i2,i2end
 * returns each pair<i1,i2> in the range exactly once.
 */
template <class It1, class It2>
class PairIteratorStl : public boost::iterator_facade<
    PairIteratorStl<It1,It2>,	    // curious recursion
    std::pair<typename It1::value_type, typename It2::value_type>,
    boost::forward_traversal_tag,
    std::pair<typename It1::value_type, typename It2::value_type> // reference type, make value
    > {
public:
    typedef typename PairIteratorStl<It1,It2>::reference reference;
    explicit PairIteratorStl<It1,It2>(const It1 &it1, const It2 &it2)
    : _it1(it1), _it1end(it1), _it2(it2), _it2end(it2), _orig_it2(it2) {
    }
    explicit PairIteratorStl<It1,It2>(const It1 &it1, const It1 &it1end,
	    			      const It2 &it2, const It2 &it2end)
    : _it1(it1), _it1end(it1end), _it2(it2), _it2end(it2end), _orig_it2(it2) {
	if (_it1 == _it1end || _it2 == _it2end) { // empty iteration
	    _it1 = _it1end;
	    _it2 = _it2end;
	}
	reinit_it1();
    }
private:
    friend class boost::iterator_core_access;
    It1 _it1, _it1end;
    It2 _it2, _it2end, _orig_it2;
    
    void increment() {
	_it2++;
	if (_it2 == _it2end) {
	    _it1++;
	    reinit_it1();
	}
    }
    bool equal(const PairIteratorStl<It1,It2> &other) const {
	if (this->_it1 != other._it1) return false;
	if (this->_it2 != other._it2) return false;
	if (this->_it1end != other._it1end) return false;
	if (this->_it2end != other._it2end) return false;
	return true;

    }
    reference dereference() const {
	return std::make_pair(*_it1, *_it2);
    }

    void reinit_it1() {
	_it2 = _it2end;
	if (_it1 != _it1end)
	    _it2 = _orig_it2;
    }
};

}; // namespace;

#endif // SEQUOIA_PAIR_ITERATOR_STL