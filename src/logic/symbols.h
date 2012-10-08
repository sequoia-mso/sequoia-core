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
#ifndef SEQUOIA_LOGIC_SYMBOLS_H
#define	SEQUOIA_LOGIC_SYMBOLS_H

#include "../config.h"
#include "../hashing.h"
#include "../unordered_defs.h"

#include <assert.h>
#include <iostream>
#include <string.h>
#include <boost/functional/hash.hpp>

namespace sequoia {

/**
 * A symbol of a vocabulary consists of an identifier, and two associated
 * integers, its arity and its nesting depth.
 * @param id The identifier of the symbol.
 * @param arity The symbol's arity.
 * @param nesting_depth The symbol's nesting depth.
 */
class Symbol {
public:
    Symbol(const char *id, unsigned int arity, bool variable,
	   unsigned int nesting_depth)
    : _identifier(id), _arity(arity), _variable(variable),
      _nesting_depth(nesting_depth) {
        assert(id != NULL);
        compute_hash();
    }
    /**
     * @return the symbol's identifier.
     */
    const char * identifier() const { return _identifier; }
    /**
     * @return the symbol's arity.
     */
    unsigned int arity() const { return _arity; }
    bool variable() const { return _variable; }
    /**
     * @return the symbol's nesting depth.
     */
    unsigned int nesting_depth() const { return _nesting_depth; }
    /**
     * Two symbols are considered equal iff their nesting depth,
     * their arity and their identifier are equal.
     * @param other The other symbol to compare with.
     * @return 
     */
    bool operator==(const Symbol &other) const {
        if (_nesting_depth != other._nesting_depth) return false;
	assert(_variable == other._variable); // must be true if the nesting depth is the same
        if (_arity != other._arity) return false;
        return ::strcmp(_identifier, other._identifier) == 0;
    }
    bool operator!=(const Symbol &s) const { return !(*this == s); }
    /**
     * @return The symbol's hash value.
     */
    size_t hash() const { return _hash; }
private:
    const char *_identifier;
    unsigned int _arity;
    bool _variable;
    unsigned int _nesting_depth;
    size_t _hash;
    void compute_hash() {
        _hash = hash_init();
        hash_combine(_hash, hash_value(_identifier));
        hash_combine(_hash, _variable);
        hash_combine(_hash, _nesting_depth);
        hash_combine(_hash, _arity);
    }
};

/**
 * A unary symbol is a Symbol of arity one.
 */
class UnarySymbol : public Symbol {
public:
    UnarySymbol(const char *id, unsigned int nesting_depth, bool variable)
    : Symbol(id, 1, variable, nesting_depth) {
        assert(id != NULL || (strlen(id) > 0 && isupper(id[0])));
    }
};

/**
 * A constant symbol is a Symbol of arity zero.
 */
class ConstantSymbol : public Symbol {
public:
    ConstantSymbol(const char *id, unsigned int nesting_depth, bool variable)
    : Symbol(id, 0, variable, nesting_depth) {
        assert(id != NULL || (strlen(id) > 0 && islower(id[0])));
    }
};

typedef SEQUOIA_UNORDERED_SET<
    const Symbol*,
    ptr_deep_hasher<Symbol>,
    ptr_deep_equals<const Symbol *>
> SymbolSet;

} // namespace
#endif	/* SEQUOIA_LOGIC_SYMBOLS_H */
