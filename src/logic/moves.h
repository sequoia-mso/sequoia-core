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
#ifndef SEQUOIA_LOGIC_MOVES_H
#define SEQUOIA_LOGIC_MOVES_H

#include "../common.h"
#include "../config.h"
#include "formula.h"

#include <sstream>
#include <boost/type_traits/is_base_of.hpp>

namespace sequoia {

/**
 * A Move class represents a move of a player in the model checking game.
 * The reader may understand a "move" as an "interpretation" of a symbol (a
 * variable) in a structure.  
 */
class Move {
public:
    Move() : _hash(0UL) { }
    Move(const Move &other) : _hash(0UL) { } // to be recomputed
    virtual std::string toString() const = 0;
    size_t hash() const {
        if (_hash == 0UL)
	    (const_cast<Move *>(this))->compute_hash();
        return _hash;
    }
protected:
    virtual void compute_hash() = 0;
    void hash(size_t hash) { _hash = hash; }
private:
    size_t _hash;
};

/**
 * The SetMove class represents a set move of a player in the model checking game.
 */
class SetMove : public Move {
public:
    typedef UnarySymbol symbol_type;
    
    SetMove() : _symbol(NULL), _size(0) { }
    SetMove(const UnarySymbol* symbol) : _symbol(symbol), _size(0) { }
    SetMove(const SetMove& other)
    : Move(other), _symbol(other._symbol), _members(other._members),
      _size(other._size) { }
    const UnarySymbol* symbol() const { return _symbol; }
    /**
     * Returns the nesting depth of this move, which is just the nesting
     * depth of the symbol.
     * @return 
     */
    unsigned int nesting_depth() const { return _symbol->nesting_depth(); }
    /**
     * Add the object with the given nesting depth.
     * Prerequisite:  It is not already contained (result is undefined otherwise).
     * @param s The object to add.
     */
    void add(unsigned int n) {
	assert(n < 64);
        assert(n < nesting_depth());
        assert(!test(n));
        _members.set(n);
        ++_size;
        hash(0UL);
    }
    void add(const ConstantSymbol* s) {
        assert(s != NULL);
        add(s->nesting_depth());
        hash(0UL);
    }
    /**
     * Remove the object with the given nesting depth.
     * Prerequisite:  It is contained (result is undefined otherwise).
     * @param s The object to remove.
     */
    void remove(unsigned int n) {
	assert(n < 64);
        assert(n < nesting_depth());
        assert(test(n));
        _members.set(n, 0);
        --_size;
        hash(0UL);
    }
    void remove(const ConstantSymbol* s) {
        assert(s != NULL);
        remove(s->nesting_depth());
    }
    /**
     * Test membership of the object with the given nesting depth.
     * @param s The symbol to test.
     */
    bool test(unsigned int n) const {
	assert(n < 64);
        return _members.test(n);
    }
    bool test(const ConstantSymbol* s) const {
        assert(s != NULL);
        return test(s->nesting_depth());
    }
    /**
     * @return The number of objects having smaller nesting depth contained.
     */
    size_t size() const { return _size; }
    /**
     * Introduce the given symbol.
     * @param s The symbol to introduce.
     */
    void introduce(const ConstantSymbol* s, bool setbit, unsigned int signature_depth) {
	assert(signature_depth > 0);
        unsigned int depth = s->nesting_depth();
        assert(depth <= signature_depth);
	if (depth == signature_depth) {
            assert(test(depth) == false);
	} else {
	    rename_obj_up(depth, signature_depth - 1);
	}
	if (setbit) add(depth);
        hash(0UL);
    }
    /**
     * Forget the given symbol.
     * @param s The symbol to introduce.
     */
    void forget(const ConstantSymbol* s, unsigned int signature_depth) {
        unsigned int depth = s->nesting_depth();
        assert(depth <= signature_depth);
  	if (test(depth))
            remove(depth);
	if (depth == signature_depth)
		return;
        rename_obj_down(depth+1, signature_depth);
        hash(0UL);
    }
    /**
     * Convert into human-readable string representation.
     * @return A string representation of this object.
     */
    std::string toString() const {
        std::stringstream outstr;
        outstr << "[SetMove"
                << "@" << this << ":"
                << "glob_nest=" << nesting_depth()
                << ",size=" << _size
                << ",members=" << _members << "]";
        return outstr.str();
    }
    bool operator==(const SetMove &s) const {
        if (hash() != s.hash()) return false;
	if (_symbol != s._symbol) return false;
        if (_members != s._members) return false;
        assert(_size == s._size); // must correlate with _members
        return true;
    }
    bool operator!=(const SetMove &s) const { return !(*this == s); }
private:
    /**
     * Perform mass renaming operation.  All constant symbols
     * from from_depth ... to_depth are renamed to
     * from_depth+1 .. to_depth+1, respectively.
     * @param from_depth
     * @param to_depth
     * @return
     */
    void rename_obj_up(unsigned int from_depth, unsigned int to_depth) {
	assert(to_depth > 0);
        assert(to_depth - 1 < nesting_depth());
        assert(to_depth >= from_depth);
        for (unsigned int i = to_depth+1; i > from_depth; --i)
            rename_obj(i-1, i);
        hash(0UL);
    }
    /**
     * Perform mass renaming operation.  All constant symbols
     * from from_depth ... to_depth are renamed to
     * from_depth-1 .. to_depth-1, respectively.
     * @param from_depth
     * @param to_depth
     * @return
     */
    void rename_obj_down(unsigned int from_depth, unsigned int to_depth) {
        assert(to_depth >= from_depth);
        assert(from_depth > 0);
        for (unsigned int i = from_depth; i <= to_depth; ++i)
            rename_obj(i, i-1);
        hash(0UL);
    }
    /**
     * Apply renaming operation of constants.
     * @param from
     * @param to
     */
    void rename_obj(unsigned int from, unsigned int to) {
        assert(from < nesting_depth());
        assert(to < nesting_depth());
        assert(from != to);
        assert(!test(to));
        if (test(from)) {
            add(to);
            remove(from);
        }
        hash(0UL);
    }

    void compute_hash() {
        size_t h = hash_init();
	hash_combine(h, _symbol->hash());
	hash_combine(h, nesting_depth());
	hash_combine(h, _size);
        for (size_t i = 0; i < nesting_depth(); ++i)
	    hash_combine(h, _members.test(i));
        hash(h);
    }
    const UnarySymbol* _symbol;
    BitSet _members;
    size_t _size;
};

/**
 * The PointMove class represents a point move of a player in the model
 * checking game.
 * It knows its relation to previous round's moves, i.e., in
 * which sets of lower nesting depth it is contained and
 * to which objects of lower nesting depths it is adjacent.
 */
class PointMove : public Move {
public:
    typedef ConstantSymbol symbol_type;
    PointMove() : _symbol(NULL) { }
    PointMove(const ConstantSymbol* symbol)
    : _symbol(symbol) { }
    PointMove(const PointMove &other)
    : Move(other), _symbol(other._symbol), _labels(other._labels),
      _edges(other._edges) { }

    const ConstantSymbol* symbol() const { return _symbol; }
    /**
     * Returns the nesting depth of this move, which is just the nesting
     * depth of the symbol.
     * @return 
     */
    unsigned int nesting_depth() const { return _symbol->nesting_depth(); }
    /**
     * Add the object with the given nesting depth.
     * Prerequisite:  It is not already contained (result is undefined otherwise).
     * @param s The object to add.
     */
    /**
     * Mark this object as being contained in the set with the given depth.
     * @param n the depth
     */
    void add_label(unsigned int n) {
	assert(n < 64);
        assert(n < nesting_depth());
        assert(!_labels.test(n));
        _labels.set(n);
        hash(0UL);
    }
    void add_label(const UnarySymbol *s) {
	assert(s != NULL);
	add_label(s->nesting_depth());
        hash(0UL);
    }
    /**
     * Mark this object as not being contained in the set s.
     * @param s The UnarySymbol of the set.
     */
    void remove_label(unsigned int n) {
	assert(n < 64);
        assert(n < nesting_depth());
        assert(_labels.test(n));
        _labels.reset(n);
        hash(0UL);
    }
    /**
     * Test whether this object is contained in the set s;
     * @param s The UnarySymbol of the set.
     * @return true iff the object is contained in the set.
     */
    bool test_label(unsigned int n) const {
	assert(n < 64);
	return _labels.test(n);
    }
    bool test_label(const UnarySymbol *s) const {
	assert(s != NULL);
	return test_label(s->nesting_depth());
    }
    /**
     * Mark this object as being adjacent to the object s.
     * @param s The ConstantSymbol of the object.
     */
    void add_edge(unsigned int n) {
	assert(n < 64);
        assert(n < nesting_depth());
        assert(!_edges.test(n));
        _edges.set(n);
        hash(0UL);
    }
    void add_edge(const ConstantSymbol *s) {
	assert(s != NULL);
	add_edge(s->nesting_depth());
    }
    /**
     * Mark this object as not adjacent to the object s.
     * @param n The ConstantSymbol of the object.
     */
    void remove_edge(unsigned int n) {
	assert(n < 64);
        assert(n < nesting_depth());
        assert(_edges.test(n));
        _edges.reset(n);
        hash(0UL);
    }
    /**
     * Test whether this object is adjacent to the object s.
     * @param n The ConstantSymbol of the object.
     * @return true iff this object is adjacent to the other object.
     */
    bool test_edge(unsigned int n) const {
        assert(n <= nesting_depth());
        return _edges.test(n);
    }
    bool test_edge(const ConstantSymbol *s) const {
	assert(s != NULL);
	return test_edge(s->nesting_depth());
    }
    /**
     * Test whether this object is a terminal.
     * Deprecated.
     * @return true iff the object is a terminal.
     */
    bool terminal() const { return !_symbol->variable(); }
    /**
     * Forget the given symbol.
     * @param s The symbol to forget.
     */
    void forget(const ConstantSymbol* s, unsigned int signature_depth) {
	assert(signature_depth > 0); // impossible to rename in an empty signature
        assert(s->nesting_depth() <= signature_depth); // must be allowed to rename this
	assert(this->symbol()->variable()); // this is not renamed, to it must be a variable
        assert(nesting_depth() > signature_depth); // variables are not part of the signature
        /* first remove edge to the forgotten node, if any */
        if (test_edge(s->nesting_depth()))
            remove_edge(s->nesting_depth());
        /* then rename objects */
        rename_obj_down(s->nesting_depth()+1, signature_depth);
        hash(0UL);
    }

    /**
     * Renames this terminal symbol from t_i to t_{i-1} due to a forget
     */
    PointMove* rename_forget(const ConstantSymbol* forget_s, const ConstantSymbol* new_s) const {
	assert(nesting_depth() > 0);
	assert(new_s->nesting_depth() == nesting_depth() - 1);
	assert(symbol() != new_s);
	assert(forget_s->nesting_depth() < this->nesting_depth());
        unsigned int forget_depth = forget_s->nesting_depth();
        PointMove *res = new PointMove(*this);
        /* first remove edge to the forgotten node, if any */
        if (res->test_edge(forget_depth))
            res->remove_edge(forget_depth);
        /* then rename other objects between me and the forgotten one */
	if (forget_s->nesting_depth() < nesting_depth())
	    res->rename_obj_down(forget_s->nesting_depth() + 1, nesting_depth()-1);
	/* finally update symbol */
        res->symbol(new_s);
        return res;
    }

    /**
     * Introduce the given symbol.
     * @param s The symbol to introduce.
     */
    void introduce(const ConstantSymbol* s, unsigned int signature_depth) {
	assert(signature_depth > 0); // impossible to rename in an empty signature
        assert(s->nesting_depth() <= signature_depth); // must be allowed to rename this
	assert(this->symbol()->variable()); // this is not renamed, to it must be a variable
        assert(nesting_depth() > signature_depth); // variables are not part of the signature
	assert(test_edge(signature_depth) == false); // make sure there is not edge set at the upper end
	rename_obj_up(s->nesting_depth(), signature_depth-1);
        hash(0UL);
    }

    /**
     * Renames this pointmove from intro_s to new_s due to an introduce.
     * We require intro_s->nesting_depth() == this->nesting_depth() and
     * new_s->nesting_depth() = intro_s->nesting_depth() + 1
     */
    PointMove* rename_introduce(const ConstantSymbol* intro_s, const ConstantSymbol* new_s, bool adjacent) const {
#if 0
	std::cout << "rename_introduce on " << toString() << " with "
		<< intro_s->nesting_depth() << "/" << new_s->nesting_depth() << " and " << adjacent << std::endl;
#endif
	assert(new_s->nesting_depth() == nesting_depth() + 1);
	assert(intro_s != new_s);
	assert(intro_s->nesting_depth() <= this->nesting_depth());
        PointMove *res = new PointMove(*this);
	res->symbol(new_s);
        /* rename other objects between intro_depth and ourselves */
	if (intro_s->nesting_depth() < nesting_depth()) {
		res->rename_obj_up(intro_s->nesting_depth(), nesting_depth()-1);
	}
	/* finally set edge to new symbol */
	if (adjacent) res->add_edge(intro_s);
        return res;
    }

    /**
     * Renames this pointmove from intro_s to new_s due to an introduce.
     * We require intro_s->nesting_depth() == this->nesting_depth() and
     * new_s->nesting_depth() = intro_s->nesting_depth() + 1
     */
    PointMove* rename_introduce(const UnarySymbol* intro_s, const ConstantSymbol* new_s, bool member) const {
#if 0
	std::cout << "rename_introduce on " << toString() << " with "
		<< intro_s->nesting_depth() << "/" << new_s->nesting_depth() << " and " << member << std::endl;
#endif
	assert(new_s->nesting_depth() == nesting_depth() + 1);
	assert(intro_s->nesting_depth() <= this->nesting_depth());
        PointMove *res = new PointMove(*this);
	res->symbol(new_s);
        /* rename other objects between intro_depth and ourselves */
	if (intro_s->nesting_depth() < nesting_depth()) {
		res->rename_obj_up(intro_s->nesting_depth(), nesting_depth()-1);
	}
	/* finally set edge to new symbol */
	if (member) res->add_label(intro_s);
        return res;
    }

    void symbol(const ConstantSymbol* symbol) {
	assert(_symbol != NULL);
	_symbol = symbol;
        hash(0UL);
    }

    std::string toString() const {
        std::stringstream outstr;
        outstr << std::string("[PointMove")
                << "@" << this
                << "::hash=" << hash() 
                << ",glob_nest=" << nesting_depth()
                << ",term=" << terminal()
                << ",lab=" << _labels
                << ",edges=" << _edges
                << "]";
        return outstr.str();
    }

    bool operator==(const PointMove &m) const {
        if (hash() != m.hash()) return false;
	if (_symbol != m._symbol) return false;
        if (_labels != m._labels) return false;
        if (_edges != m._edges) return false;
        return true;
    };
    bool operator!=(const PointMove &s) const { return !(*this == s); };
private:
    /**
     * Perform mass renaming operation.  All constant symbols
     * from from_depth ... to_depth are renamed to
     * from_depth+1 .. to_depth+1, respectively.
     * @param from_depth
     * @param to_depth
     * @return
     */
    void rename_obj_up(int from_depth, int to_depth) {
	if (to_depth == 0) return; // nothing to rename
	assert(to_depth - 1 < (int) nesting_depth());
        for (int i = to_depth+1; i > from_depth; --i)
            rename_obj(i-1, i);
        hash(0UL);
    }

    /**
     * Perform mass renaming operation.  All constant symbols
     * from from_depth ... to_depth are renamed to
     * from_depth-1 .. to_depth-1, respectively.
     * @param from_depth
     * @param to_depth
     * @return
     */
    void rename_obj_down(unsigned int from_depth, unsigned int to_depth) {
        assert(from_depth > 0);
	assert(to_depth < nesting_depth());
        for (unsigned int i = from_depth; i <= to_depth; ++i)
            rename_obj(i, i - 1);
        hash(0UL);
    }


    /**
     * Rename constant symbols and adjust the nesting depths accordingly.
     * This possibly includes the current object.
     * @param from
     * @param to
     */
    void rename_obj(unsigned int from, unsigned int to) {
	assert(from != nesting_depth());
	assert(to != nesting_depth());
        assert(from != to);
        assert(_edges.test(to) == false);
        if (_edges.test(from)) {
            remove_edge(from);
            if (to <= nesting_depth()) {
                add_edge(to);
            }
        }
        hash(0UL);
    }

    void compute_hash() {
        size_t h = hash_init();
	hash_combine(h, _symbol->hash());
	hash_combine(h, nesting_depth());
        for (size_t i = 0; i < nesting_depth(); ++i)
	    hash_combine(h, _edges.test(i));
        for (size_t i = 0; i < nesting_depth(); ++i)
	    hash_combine(h, _labels.test(i));
        hash(h);
    }
    const ConstantSymbol* _symbol;
    BitSet _labels, _edges;
};

/*
 * Type trait to select different implementations for point/set moves
 */
template <typename T> 
struct is_pointmove : boost::is_base_of<PointMove, T> { }; 
template <typename T> 
struct is_setmove : boost::is_base_of<PointMove, T> { }; 

} // namespace

#endif
