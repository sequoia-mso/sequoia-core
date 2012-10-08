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
#ifndef SEQUOIA_EVAL_MINMAXCARDSET_H
#define SEQUOIA_EVAL_MINMAXCARDSET_H

#include "sequoia_eval.h"
#include "unordered_defs.h"

#ifdef HAVE_TBB
#include <tbb/concurrent_vector.h>
#define CONCUR_VECTOR tbb::concurrent_vector
#else
#include <vector>
#define CONCUR_VECTOR std::vector
#endif

namespace sequoia {

/**
 * The MinMaxCardEvaluation finds the size of a minimum/maximum cardinality solution
 * for the first free set variable.
 * It only saves the concrete solutions for base elements (elem()), and
 * uses composed elements pointing to two sub-elements for multiplication.
 * Multiplication:  result is union of two (partial) solutions.
 * Addition: result is the smaller (partial) solution.
 */
template <typename Tester>
class MinMaxCardSetEvaluation : public SequoiaInternalEvaluation {
private:
    class value_t {
    public:
	virtual ~value_t() { }
        typedef SEQUOIA_UNORDERED_SET<unsigned int> vertex_set;
        value_t(size_t s) : _size(s) { }
        size_t size() const { return _size; }
	virtual vertex_set get() const = 0;
    private:
        size_t _size;
    };

    class base_value_t : public value_t {
    public:
	typedef typename MinMaxCardSetEvaluation<Tester>::value_t::vertex_set vertex_set;
        base_value_t(size_t s, const vertex_set& set) : value_t(s), _set(set) { }
	virtual vertex_set get() const { return _set; }
	virtual ~base_value_t() { }
    private:
        vertex_set _set;
    };

    class composed_value_t : public value_t {
    public:
	typedef typename MinMaxCardSetEvaluation<Tester>::value_t::vertex_set vertex_set;
        composed_value_t(size_t s, const value_t* p1, const value_t* p2)
	: value_t(s), _p1(p1), _p2(p2) { }
	virtual vertex_set get() const {
	    vertex_set out = _p1->get();
	    vertex_set s2 = _p2->get();
	    out.insert(s2.begin(), s2.end());
	    return out;
	}
	virtual ~composed_value_t() { }
    private:
        const value_t* _p1;
        const value_t* _p2;
    };
public:
    ~MinMaxCardSetEvaluation<Tester>() {
	typename CONCUR_VECTOR<const value_t*>::const_iterator it;
	for (it = _values.begin(); it != _values.end(); it++) {
	    delete *it;
	}
    }

    const void* elem(const sequoia::Bag *bag,
                     const sequoia::Assignment *alpha) {
        assert(_free_symbols.size() > 0);
        const sequoia::SetMove *smove = alpha->get(_free_symbols[0]);
	size_t size = 0;
	typename base_value_t::vertex_set set;
        for (unsigned int i = 0; i < _terminal_symbols.size(); ++i) {
            const sequoia::ConstantSymbol* t = _terminal_symbols[i];
	    if (smove->test(t)) {
		size++;
		unsigned int t_num = _terminal_symbols[i]->nesting_depth()
                    - _terminal_symbols[0]->nesting_depth();
		set.insert(bag->get(t_num));
            }
        }
	const base_value_t *result = new base_value_t(size, set);
	_values.push_back(result);
	return result;
    }

    const void* mult(const void *e1, const void *e2, const void *intersect) {
        const value_t *val1 = (const value_t *) e1;
        const value_t *val2 = (const value_t *) e2;
        const value_t *is = (const value_t *) intersect;
	size_t size = val1->size() + val2->size() - is->size();
	const composed_value_t *result = new composed_value_t(size, val1, val2);
	_values.push_back(result);
	return result;
    }

    const void* add(const void *e1, const void *e2) {
        const value_t *val1 = (const value_t *) e1;
        const value_t *val2 = (const value_t *) e2;
	Tester tester;
        if (tester(val1->size(), val2->size()))
            return val1;
        else
            return val2;
    }

    void output_solution(std::ostream &outs, const void *e) const {
	const value_t *val = (const value_t *) e;
        outs << "size: " << val->size() << ", members: ";
	typename value_t::vertex_set set = val->get();
	typename value_t::vertex_set::const_iterator it;
        for (it = set.begin(); it != set.end(); ++it)
            outs << (it == set.begin() ? "" : ", ") << *it;
    }

    bool returns_vertex_sets() const {
        return true;
    }

    vertex_sets_t
    convert_to_vertex_sets(const void *e) const {
        vertex_sets_t res;
        res.push_back(std::set<int>());

        const value_t *val = (const value_t *) e;
	typename value_t::vertex_set set = val->get();
	typename value_t::vertex_set::const_iterator it;
        for (it = set.begin(); it != set.end(); ++it)
            res[0].insert(*it);

        // fill in sets we did not evaluate
        for (int i = 1; i < _free_symbols.size(); i++)
            res.push_back(std::set<int>());
        return res;
    }

private:
     CONCUR_VECTOR<const value_t*> _values;
};

} // namespace

#endif // SEQUOIA_EVAL_MINMAXCARDSET_H