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
#ifndef SEQUOIA_STRUCTURES_BAG_H
#define SEQUOIA_STRUCTURES_BAG_H

#include <assert.h>
#include <iostream>
#include <vector>

namespace sequoia {

class Bag {
    typedef size_t index_t;
    typedef unsigned int vertex_t;
    typedef std::vector<vertex_t> Container;
    typedef Container::const_iterator const_iterator;
public:
    Bag() : _elements() { }
    Bag(const Bag& other) : _elements(other._elements) { }

    /**
     * @return number of elements
     */
    size_t width() const { return _elements.size(); }

    /**
     * @param i the element's index to return
     * @return the i-th element in the bag
     */
    unsigned int get(index_t i) const {
        assert(i < _elements.size());
        return _elements[i];
    }

    /**
     * Test membership
     * @param v the vertex to test for
     * @return true iff vertev v is a member of the bag
     */
    bool member(unsigned int v) const {
        const_iterator b = _elements.begin();
        const_iterator bend = _elements.end();
        for (; b != bend; b++)
            if (*b == v)
                return true;
        return false;
    }
    bool is_subset_of(const Bag& other) const {
        // TODO: improve: can be done in linear time, since both bags are sorted
        const_iterator b = _elements.begin();
        const_iterator bend = _elements.end();
        for (; b != bend; b++)
            if (!other.member(*b))
                return false;
        return true;
    }
    int missing_terminal(const Bag &super) const {
	assert(is_subset_of(super));
	for (size_t i = 0; i < super.width(); ++i)
	    if (!this->member(super.get(i)))
		return i;
	return -1;
    }
    void add(const int v) {
        _elements.push_back(v);
        sort(_elements.begin(), _elements.end());
    }

    friend std::ostream& operator<<(std::ostream& outs, const Bag& bag) {
        outs << "Bag(";
        for (size_t i = 0; i < bag._elements.size(); ++i)
            outs << (i > 0 ? ", " : "") << bag.get(i);
        outs << ")";
        return outs;
    }
    bool operator==(const Bag& other) const {
        return _elements == other._elements;
    }
    bool operator!=(const Bag& other) const {
        return _elements != other._elements;
    }
    Bag& operator=(const Bag& other) {
        _elements = other._elements;
        return *this;
    }
private:
    Container _elements;
};

} // namespace

#endif // SEQUOIA_STRUCTURES_BAG_H