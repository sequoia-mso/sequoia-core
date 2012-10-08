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
#ifndef SEQUOIA_LOGIC_VOCABULARY_H
#define	SEQUOIA_LOGIC_VOCABULARY_H

#include "symbols.h"
#include "../config.h"

#include <sstream>
#include <string.h>
#include <vector>

namespace sequoia {

/**
 * The Vocabulary class represents a vocabulary (for structures and formulas).
 * A vocabulary is an ordered list of symbols of arbitrary arity.
 * For constant and unary symbols we provide several special functions.
 */
class Vocabulary {
public:
    Vocabulary() { }
    /**
     * Create a copy of the other vocabulary.
     * Beware:  This is not a deep copy, i.e., the copy uses the same
     * (pointers to) symbols than the original vocabulary.
     */
    Vocabulary(const Vocabulary *other) :
            _symbols(other->_symbols),
            _constant_symbols(other->_constant_symbols),
            _unary_symbols(other->_unary_symbols) { }
    /**
     * @return The number of symbols in this vocabulary.
     */
    unsigned int size() const { return _symbols.size(); }
    /**
     * @return The number of constant symbols in this vocabulary.
     */
    unsigned int number_of_constant_symbols() const {
        return _constant_symbols.size();
    }
    /**
     * @return The number of unary symbols in this vocabulary.
     */
    unsigned int number_of_unary_symbols() const {
        return _unary_symbols.size();
    }
    /**
     * Returns the i-th symbol.  Note that i must be smaller than size(),
     * otherwise a severe error is likely.
     * @return The i-th symbol in the vocabulary.
     */
    const Symbol* symbol(unsigned int i) const {
        assert(i < size());
        return _symbols[i];
    }
    /**
     * Returns the i-th constant symbol.  Note that i must be smaller than
     * number_of_constant_symbols(), otherwise a severe error is likely.
     * @return The i-th constant symbol in the vocabulary.
     */
    const ConstantSymbol* constant_symbol(unsigned int i) const {
        assert(i < number_of_constant_symbols());
        return _constant_symbols[i];
    }
    /**
     * Returns the i-th unary symbol.  Note that i must be smaller than
     * number_of_unary_symbols(), otherwise a severe error is likely.
     * @return The i-th unary symbol in the vocabulary.
     */
    const UnarySymbol* unary_symbol(unsigned int i) const {
        assert(i < number_of_unary_symbols());
        return _unary_symbols[i];
    }
    /**
     * Check whether the first size() symbols in this and other vocabulary
     * are identical.
     * @param The other vocabulary to compare this.
     * @return true iff the other vocabulary extends this one.
     */
    bool extends(const Vocabulary* other) const;
    /**
     * Check whether the other->size() first symbols in this and other
     * vocabulary * are identical.
     * @param The other Vocabulary to compare this.
     * @return true iff this vocabulary extends the other.
     */
    bool reduces(const Vocabulary* other) const;
    /**
     * Extend this vocabulary by the constant symbol sym.
     * The symbol's constant nesting depth must match the correct
     * nesting depth in this vocabulary.
     * @param sym The Symbol to be added.
     */
    void add_constant_symbol(const ConstantSymbol *sym) {
        _add_symbol(sym);
        _constant_symbols.push_back(sym);
    }
    /**
     * Extend this vocabulary by a new constant symbol with a given
     * identifier.
     * @param indentifier The identifier of the new symbol.
     */
    const ConstantSymbol* add_constant_symbol(const std::string& identifier, bool variable) {
        const char* id = ::strdup(identifier.c_str());
        ConstantSymbol* sym = new ConstantSymbol(id, size(), variable);
        add_constant_symbol(sym);
        return sym;
    }
    /**
     * Extend this vocabulary by the unary symbol sym.
     * The symbol's unary nesting depth must match the correct
     * nesting depth in this vocabulary.
     * @param sym The Symbol to be added.
     */
    void add_unary_symbol(const UnarySymbol *sym) {
        _add_symbol(sym);
        _unary_symbols.push_back(sym);
    }
    /**
     * Extend this vocabulary by a new unary symbol with a given
     * identifier.
     * @param indentifier The identifier of the new symbol.
     */
    void add_unary_symbol(const std::string& identifier, bool variable) {
        const char* id = ::strdup(identifier.c_str());
        UnarySymbol* sym = new UnarySymbol(id, size(), variable);
        add_unary_symbol(sym);
    }
    /**
     * Extend this vocabulary by a new symbol with an arity at least 2.
     * @param sym The new symbol.
     */
    void add_symbol(const Symbol* sym) {
        assert(sym->arity() > 1);
        _add_symbol(sym);
    }
    /**
     * Test whether a symbol equal to sym is contained in the vocabulary.
     * Uses a deep equality test.
     * @param sym The symbols
     * @return true iff an equal symbol is contained.
     */
    bool contains(const Symbol* sym) const {
        for (unsigned int i = 0; i < _symbols.size(); ++i)
            if (*sym == *(_symbols[i]))
                return true;
        return false;
    }
    /**
     * Extend this vocabulary by the constant symbol sym and return the result,
     * a new vocabulary.  Restrictions of add_constant_symbol() apply.
     * @return A new vocabulary, which is this extended by sym.
     */
    Vocabulary* extend_by(const ConstantSymbol *sym) const {
        Vocabulary* res = new Vocabulary(this);
        res->add_constant_symbol(sym);
        return res;
    }
    /**
     * Extend this vocabulary by the unary symbol sym and return the result,
     * a new vocabulary.  Restrictions of add_unary_symbol() apply.
     * @return A new vocabulary, which is this extended by sym.
     */
    Vocabulary* extend_by(const UnarySymbol *sym) const {
        Vocabulary* res = new Vocabulary(this);
        res->add_unary_symbol(sym);
        return res;
    }
    /**
     * Convert vocabulary in a (human-readable) string representation.
     * @return A string representation of this vocabulary.
     */
    std::string toString() const;

    bool operator==(const Vocabulary &other) const {
        if (_symbols != other._symbols) return false;
        if (_constant_symbols != other._constant_symbols) return false;
        if (_unary_symbols != other._unary_symbols) return false;
        return true;
    }
private:
    void _add_symbol(const Symbol* sym) {
        assert(sym->nesting_depth() == size());
        _symbols.push_back(sym);
    }
    std::vector<const Symbol*> _symbols;
    std::vector<const ConstantSymbol*> _constant_symbols;
    std::vector<const UnarySymbol*> _unary_symbols;
};


} // namespace

#endif	/* SEQUOIA_LOGIC_VOCABULARY_H */
