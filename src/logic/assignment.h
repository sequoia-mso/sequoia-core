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
#ifndef SEQUOIA_LOGIC_ASSIGNMENT_H
#define	SEQUOIA_LOGIC_ASSIGNMENT_H

#include "flyweight.h"
#include "flyweight_inheritance.h"
#include "moves.h"
#include "../config.h"

namespace sequoia {

class Assignment;
typedef FlyweightInheritance<Assignment> Assignment_f;
typedef FlyweightInheritanceFactory<Assignment> AssignmentFlyFactory;

/**
 * Assignments of symbols
 */
class Assignment {
public:
    Assignment() : _hash(0UL) { }
    virtual ~Assignment() { }
    size_t hash() const { assert(_hash != 0UL); return _hash; }
    virtual const PointMove* get(const ConstantSymbol* sym) const = 0;
    virtual const SetMove* get(const UnarySymbol* sym) const = 0;
    virtual bool assigned(const ConstantSymbol *sym) const = 0;
    virtual bool assigned(const UnarySymbol *sym) const = 0;
    /**
     * This one is needed for now, because forget/introduce need a _fast_ way
     * to retrieve an interpretation based on the previous' move nesting depth.
     */
    virtual const PointMove* get_pointmove(unsigned int depth) const = 0;
    virtual unsigned int depth() const = 0;
protected:
    void hash(size_t hash) { this->_hash = hash; }
private:
    size_t _hash;
};


class EmptyAssignment : public Assignment {
public:
    EmptyAssignment() { hash(1UL); }
    const PointMove* get(const ConstantSymbol* sym) const { assert(false); return NULL; }
    const SetMove* get(const UnarySymbol* sym) const { assert(false); return NULL; }
    const PointMove* get_pointmove(unsigned int depth) const { return NULL; }
    bool operator==(const EmptyAssignment &other) const { return true; }
    bool operator!=(const EmptyAssignment &other) const { return false; }
    virtual bool assigned(const ConstantSymbol *sym) const { return false; }
    virtual bool assigned(const UnarySymbol *sym) const { return false; }
    virtual unsigned int depth() const { return 0; }
};

template <typename TSymbol, typename TMove>
class SymAssignment : public Assignment {
public:
    SymAssignment<TSymbol, TMove>(const Assignment_f *parent,
                                  const TSymbol *symbol,
				  const TMove* move)
    : _parent(parent->clone()), _symbol(symbol), _move(move) {
        assert(!_parent->get()->assigned(_symbol)); // for debugging
        size_t h = hash_init();
        hash_combine(h, _parent->get()->hash());
        hash_combine(h, _symbol->hash());
        if (_move != NULL)
            hash_combine(h, _move->hash());
        hash(h);
    }
    ~SymAssignment<TSymbol, TMove>() {
	delete _parent;
    }
    const PointMove* get(const ConstantSymbol* sym) const {
        if (boost::is_base_of<TSymbol, ConstantSymbol>::value) // optimized out by compiler
            if (sym == (const ConstantSymbol*) _symbol) return (const PointMove*) _move;
        return _parent->get()->get(sym);
    }
    const SetMove* get(const UnarySymbol *sym) const {
        if (boost::is_base_of<TSymbol, UnarySymbol>::value) // optimized out by compiler
            if (sym == (const UnarySymbol*) _symbol) return (const SetMove*) _move;
        return _parent->get()->get(sym);
    }

    const PointMove* get_pointmove(unsigned int depth) const {
	assert(depth <= _symbol->nesting_depth());
        if (boost::is_base_of<TSymbol, ConstantSymbol>::value) // optimized out by compiler
            if (depth == _symbol->nesting_depth()) return (const PointMove*) _move;
	return _parent->get()->get_pointmove(depth);
    }

    bool operator==(const SymAssignment<TSymbol, TMove>& other) const {
        if (hash() != other.hash()) return false;
        if (_symbol != other._symbol) return false;
        if (_move != other._move) return false;
        return *_parent == *other._parent;
    }
    bool operator!=(const SymAssignment<TSymbol, TMove>& other) const {
        return !(*this == other);
    }
    virtual bool assigned(const ConstantSymbol* sym) const {
        if (boost::is_base_of<TSymbol, ConstantSymbol>::value) // optimized out by compiler
            if (sym == (const ConstantSymbol*) _symbol) return true;
        return _parent->get()->assigned(sym);
    }
    virtual bool assigned(const UnarySymbol *sym) const {
        if (boost::is_base_of<TSymbol, UnarySymbol>::value) // optimized out by compiler
            if (sym == (const UnarySymbol*) _symbol) return true;
        return _parent->get()->assigned(sym);
    }
    virtual unsigned int depth() const { return _symbol->nesting_depth(); }
private:
    const Assignment_f *_parent;
    const TSymbol *_symbol;
    const TMove *_move;
};

typedef SymAssignment<UnarySymbol, SetMove> SetAssignment;
typedef SymAssignment<ConstantSymbol, PointMove> ObjAssignment;

} // namespace

#endif	/* SEQUOIA_ASSIGNMENT_H */

