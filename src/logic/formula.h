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
#ifndef SEQUOIA_LOGIC_FORMULA_H
#define SEQUOIA_LOGIC_FORMULA_H

#include "formula_visitor.h"
#include "vocabulary.h"
#include "../config.h"

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <boost/tuple/tuple.hpp> // boost::tie
#include <boost/iterator/iterator_facade.hpp>

namespace sequoia {

/**
 * Base class for Formulas.
 * Each formula is over a given vocabulary
 * @param vocabulary
 */
class Formula {
public:
    Formula(const Vocabulary *vocabulary)
    :   _vocabulary(vocabulary), _height(1L), _size(1L), _hash(0UL) { }
    virtual void accept(FormulaVisitor* visitor) const = 0;
    virtual std::string toString() const { return typeid(*this).name(); }
    size_t hash() const { assert(_hash != 0UL); return _hash; }
    const Vocabulary* vocabulary() const { return _vocabulary; }
    std::ostream& operator<<(std::ostream& s) const {
	s << toString();
	return s;
    }
    size_t height() const { return _height; }
    size_t size() const { return _height; }
private:
    const Vocabulary* _vocabulary;
    unsigned short _height;
    unsigned short _size;
    size_t _hash;
protected:
    size_t compute_hash() {
        size_t h = hash_init();
	hash_combine(h, _size);
	hash_combine(h, _height);
	return h;
    }
    void hash(size_t hash) { _hash = hash; }
    void max_height(unsigned short h) { _height = std::max(_height, h); }
    void add_size(unsigned short s) { _size += s; }
};

/**
 * A quantified formula 
 * @param vocabulary The formula's vocabulary
 * @param variable The variable/symbol subject to quantification
 * @param subformula The subformula with vocabulary vocabulary+symbol
 */
class QFormula : public Formula {
public:
    QFormula(const Vocabulary* vocabulary, const Symbol* variable, const Formula* subformula)
    : Formula(vocabulary), _subformula(subformula) {
        assert(variable != NULL);
        assert(subformula != NULL);
        assert(subformula->vocabulary() != NULL);
        assert(vocabulary != NULL);
        assert(subformula->vocabulary()->extends(vocabulary));
        int s = subformula->vocabulary()->size();
        assert(subformula->vocabulary()->symbol(s-1) == variable);
        max_height(subformula->height()+1);
        add_size(subformula->size());
    }
    class subformula_iterator : public boost::iterator_facade<
    	subformula_iterator,
	const Formula* const, 
	boost::forward_traversal_tag,
	const Formula* const> {
    public:
	subformula_iterator() { } 
	explicit subformula_iterator(const Formula* p) : _ptr(p) { }
    private:
	friend class boost::iterator_core_access;
	void increment() { _ptr = NULL; }
	bool equal(subformula_iterator const& other) const {
	    return this->_ptr == other._ptr;
	}
    	const Formula* dereference() const { return _ptr; }
	const Formula* _ptr;
    };
    typedef std::pair<subformula_iterator, subformula_iterator> subformulas_range_t;
    subformulas_range_t subformulas() const {
	return std::make_pair(subformula_iterator(subformula()), subformula_iterator(NULL));
    }
    subformula_iterator subformulas_begin() const { return subformula_iterator(subformula()); }
    subformula_iterator subformulas_end() const { return subformula_iterator(NULL); }

protected:
    const Formula* subformula() const { return _subformula; }
private:
    const Formula* _subformula;
};

/**
 * A set quantification formula.
 * @param vocabulary
 * @param variable
 * @param subformula
 */
class SetQFormula : public QFormula {
public:
    SetQFormula(const Vocabulary* vocabulary, const UnarySymbol* variable, const Formula *subformula)
    :	QFormula(vocabulary, variable, subformula), _variable(variable) {
        compute_hash();
    }
    const UnarySymbol * variable() const { return _variable; }
private:
    void compute_hash() {
        size_t h = Formula::compute_hash();
        hash_combine(h, subformula()->hash());
        hash_combine(h, variable()->hash());
	hash(h);
    }
    const UnarySymbol* _variable;
};
class UnivSetQFormula : public SetQFormula {
public:
    UnivSetQFormula(const Vocabulary* vocabulary, const UnarySymbol* variable, const Formula *subformula)
    :   SetQFormula(vocabulary, variable, subformula) { }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    virtual std::string toString() const {
        return std::string("ALL ")
            + variable()->identifier() + " (" + subformula()->toString() + ")";
    }
};
class ExistSetQFormula : public SetQFormula {
public:
    ExistSetQFormula(const Vocabulary* vocabulary, const UnarySymbol* variable, const Formula *subformula) 
    :	SetQFormula(vocabulary, variable, subformula) { }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    virtual std::string toString() const {
        return std::string("EX ")
            + variable()->identifier() + " (" + subformula()->toString() + ")";
    }
};

/**
 * An object quantification formula.
 * @param vocabulary
 * @param variable
 * @param subformula
 */
class ObjQFormula : public QFormula {
public:
    ObjQFormula(const Vocabulary* vocabulary, const ConstantSymbol *variable, const Formula *subformula)
    :	QFormula(vocabulary, variable, subformula), _variable(variable) {
        compute_hash();
    }
    const ConstantSymbol * variable() const { return _variable; }
private:
    void compute_hash() {
        size_t h = Formula::compute_hash();
        hash_combine(h, subformula()->hash());
        hash_combine(h, variable()->hash());
	hash(h);
    }
    const ConstantSymbol* _variable;
};
class UnivObjQFormula : public ObjQFormula {
public:
    UnivObjQFormula(const Vocabulary* vocabulary, const ConstantSymbol *variable, const Formula *subformula)
    :   ObjQFormula(vocabulary, variable, subformula) { }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    virtual std::string toString() const {
        return std::string("ALL ")
            + variable()->identifier() + " (" + subformula()->toString() + ")";
    }
};
class ExistObjQFormula : public ObjQFormula {
public:
    ExistObjQFormula(const Vocabulary* vocabulary, const ConstantSymbol *variable, const Formula *subformula)
    :   ObjQFormula(vocabulary, variable, subformula) { }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    virtual std::string toString() const {
        return std::string("EX ")
            + variable()->identifier() + " (" + subformula()->toString() + ")";
    }
};

/**
 * A Boolean combination formula (conjunction or disjunction)
 */
class BoolCombFormula : public Formula {
public:
    BoolCombFormula(const Vocabulary* vocabulary) : Formula(vocabulary), _subformulas(0) {
        compute_hash();
    }
    typedef std::vector<const Formula *> subformula_list_t;
    typedef subformula_list_t::const_iterator subformula_iterator;
    typedef std::pair<subformula_iterator, subformula_iterator> subformulas_range_t;
    subformulas_range_t subformulas() const {
	return std::make_pair(_subformulas.begin(), _subformulas.end());
    }
    subformula_iterator subformulas_begin() const { return _subformulas.begin(); }
    subformula_iterator subformulas_end() const { return _subformulas.end(); }
    void add_subformula(const Formula *f) {
        assert(f->vocabulary() == vocabulary());
        _subformulas.push_back(f);
        max_height(f->height()+1);
        add_size(f->size());
        compute_hash();
    }
private:
    void compute_hash() {
        size_t h = Formula::compute_hash();
	subformula_iterator it, itend;
        for (it = subformulas_begin(); it != subformulas_end(); ++it)
            hash_combine(h, (*it)->hash());
	hash(h);
    }
    subformula_list_t _subformulas;
};
class ConjBoolCombFormula : public BoolCombFormula {
public:
    ConjBoolCombFormula(const Vocabulary* vocabulary) : BoolCombFormula(vocabulary) { }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    virtual std::string toString() const {
	std::stringstream out;
	out << "(";
	subformula_iterator it, itend;
	for (boost::tie(it, itend) = subformulas(); it != itend; it++)
	    out << (it != subformulas_begin() ? " && " : "") << (*it)->toString();
	out << ")";
        return out.str();
    }
protected:
};
class DisjBoolCombFormula : public BoolCombFormula {
public:
    DisjBoolCombFormula(const Vocabulary* vocabulary) : BoolCombFormula(vocabulary) { }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    virtual std::string toString() const {
	std::stringstream out;
	out << "(";
	subformula_iterator it, itend;
	for (boost::tie(it, itend) = subformulas(); it != itend; it++)
	    out << (it != subformulas_begin() ? " || " : "") << (*it)->toString();
	out << ")";
        return out.str();
    }
};

class AtomarFormulaMember : public Formula {
public:
    AtomarFormulaMember(const Vocabulary* vocabulary,
        const ConstantSymbol *constsym,
        const UnarySymbol *set)
        :   Formula(vocabulary),
            _constsym(constsym),
            _setsym(set) {
        assert(vocabulary->contains(constsym));
        assert(vocabulary->contains(set));
        compute_hash();
    }
    const ConstantSymbol * constant_symbol() const { return _constsym; }
    const UnarySymbol * unary_symbol() const { return _setsym; }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    std::string toString() const {
        return std::string("") + _constsym->identifier() + " in " + _setsym->identifier();
    }
private:
    void compute_hash() {
        size_t h = Formula::compute_hash();
	hash_combine(h, hash_value("member"));
        hash_combine(h, _constsym->hash());
        hash_combine(h, _setsym->hash());
	hash(h);
    }
    const ConstantSymbol *_constsym;
    const UnarySymbol *_setsym;
};
class AtomarFormulaAdj : public Formula {
public:
    AtomarFormulaAdj(const Vocabulary* vocabulary,
        const ConstantSymbol *x,
        const ConstantSymbol *y)
        :   Formula(vocabulary), _x(x), _y(y) {
        assert(vocabulary->contains(x));
        assert(vocabulary->contains(y));
        compute_hash();
    }
    const ConstantSymbol* x() const { return _x; }
    const ConstantSymbol* y() const { return _y; }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    virtual std::string toString() const {
        return std::string("adj(") + _x->identifier() + ","
                + _y->identifier() + ")";
    }
private:
    void compute_hash() {
        size_t h = Formula::compute_hash();
	hash_combine(h, hash_value("adj"));
        hash_combine(h, _x->hash());
        hash_combine(h, _y->hash());
	hash(h);
    }
    const ConstantSymbol *_x, *_y;
};
class AtomarFormulaEquals : public Formula {
public:
    AtomarFormulaEquals(const Vocabulary* vocabulary,
        const ConstantSymbol *x,
        const ConstantSymbol *y)
        :   Formula(vocabulary), _x(x), _y(y) {
        assert(vocabulary->contains(x));
        assert(vocabulary->contains(y));
        compute_hash();
    }
    const ConstantSymbol* x() const { return _x; }
    const ConstantSymbol* y() const { return _y; }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    virtual std::string toString() const {
        return std::string("") + _x->identifier() + " = " + _y->identifier();
    }
private:
    void compute_hash() {
        size_t h = Formula::compute_hash();
	hash_combine(h, hash_value("=="));
        hash_combine(h, _x->hash());
        hash_combine(h, _y->hash());
	hash(h);
    }
    const ConstantSymbol *_x, *_y;
};


class NegatedFormula : public Formula {
public:
    NegatedFormula(const Vocabulary* vocabulary,
        const Formula *subformula)
        : Formula(vocabulary), _subformula(subformula) {
        assert(_subformula != NULL);
        max_height(subformula->height() + 1);
        add_size(subformula->size());
        compute_hash();
    }
    virtual void accept(FormulaVisitor* visitor) const { visitor->visit(this); }
    virtual std::string toString() const {
        return std::string("NOT " + _subformula->toString());
    }
    const Formula* subformula() const { return _subformula; }
private:
    void compute_hash() {
        size_t h = Formula::compute_hash();
	hash_combine(h, hash_value("NOT"));
        hash_combine(h, _subformula->hash());
	hash(h);
    }
    const Formula *_subformula;
};

}

#endif // SEQUOIA_LOGIC_FORMULA_H
