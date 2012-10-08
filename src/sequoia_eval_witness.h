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
#include "../src/evaluation.h"
#include <iostream>

#include "unordered_defs.h"
#include "sequoia_eval.h"

#include <vector>
#ifdef HAVE_TBB
#include <tbb/concurrent_vector.h>
#define CONCUR_VECTOR tbb::concurrent_vector
#else
#define CONCUR_VECTOR std::vector
#endif

using namespace sequoia;

/**
 * The WitnessEvaluation outputs an arbitrary solution, i.e., an assignment
 * to the free variables for which the formula holds.  This
 * evaluation can be used to output solutions for existential problems such
 * as Hamiltonian Cycle.
 * Multiplication:  result is union of two (partial) solutions.
 * Addition: result is either of the two solutions.
 */
class WitnessEvaluation : public SequoiaInternalEvaluation {
public:
    typedef std::vector<SEQUOIA_UNORDERED_SET<unsigned int> > solution_t;
private:
    class value_t {
    public:
        virtual ~value_t() { }
        virtual solution_t get() const = 0;
    };

    class base_value_t : public value_t {
    public:
        explicit base_value_t(const solution_t& entry) : _entry(entry) { }
        virtual ~base_value_t() { }
        virtual solution_t get() const { return _entry; }
    private:
        solution_t _entry;
    };

    class composed_value_t : public value_t {
    public:
        composed_value_t(const value_t* p1, const value_t* p2) : _p1(p1), _p2(p2) { }
        virtual ~composed_value_t() { }
        virtual solution_t get() const {
            solution_t s1 = _p1->get();
            solution_t s2 = _p2->get();
	    solution_t out;
	    solution_t::const_iterator it;
	    for (it = s1.begin(); it != s1.end(); it++)
		out.push_back(*it);
	    int i = 0;
	    for (it = s2.begin(); it != s2.end(); it++)
		out[i++].insert(it->begin(), it->end());
            return out;
        }
    private:
        const value_t* _p1;
        const value_t* _p2;
    };

public:
    ~WitnessEvaluation() {
	CONCUR_VECTOR<const value_t*>::const_iterator it;
	for (it = _values.begin(); it != _values.end(); it++) {
	    delete *it;
	}
    }

    const void * elem(const sequoia::Bag *bag,
                      const sequoia::Assignment *alpha) {
	solution_t thissol;
        for (unsigned int k = 0; k < _free_symbols.size(); k++) {
            const sequoia::SetMove *smove = alpha->get(_free_symbols[k]);
	    SEQUOIA_UNORDERED_SET<unsigned int> thisset;
            // collect elements in this set
            for (unsigned int i = 0; i < _terminal_symbols.size(); ++i) {
                const sequoia::ConstantSymbol* t = _terminal_symbols[i];
                unsigned int t_num = _terminal_symbols[i]->nesting_depth()
                        - _terminal_symbols[0]->nesting_depth();
                if (smove->test(t))
                    thisset.insert(bag->get(t_num));
            }
	    thissol.push_back(thisset);
        }
        return new base_value_t(thissol);
    }

    const void * mult(const void *e1, const void *e2, const void *intersect) {
	const value_t *val1 = (const value_t *) e1;
        const value_t *val2 = (const value_t *) e2;
	const composed_value_t *result = new composed_value_t(val1, val2);
	_values.push_back(result);
	return result;
    }

    const void * add(const void *e1, const void *e2) { return e1; }
    


    void output_solution(std::ostream &outs, const void *e) const {
        const value_t *val = (const value_t *) e;
	solution_t sol = val->get();
        solution_t::const_iterator it;
        int i = 0;
        for (it = sol.begin(); it != sol.end(); it++) {
            const sequoia::UnarySymbol *svar = _free_symbols[i++];
	    const SEQUOIA_UNORDERED_SET<unsigned int> &inset = *it;
            outs << svar->identifier() << ": " << inset.size() << ", members: ";
	    SEQUOIA_UNORDERED_SET<unsigned int>::const_iterator sit;
            for (sit = inset.begin(); sit != inset.end(); sit++)
                outs << (sit == inset.begin() ? "" : ", ") << *sit;
            outs << std::endl;
        }
    }

    bool returns_vertex_sets() const {
        return true;
    }

    vertex_sets_t
    convert_to_vertex_sets(const void *e) const {
        vertex_sets_t res;
        const value_t *val = (const value_t *) e;
	solution_t sol = val->get();
        solution_t::const_iterator it;

        int current = -1;
        for (it = sol.begin(); it != sol.end(); it++) {
            res.push_back(std::set<int>());
            current++;
	    const SEQUOIA_UNORDERED_SET<unsigned int> &inset = *it;
	    SEQUOIA_UNORDERED_SET<unsigned int>::const_iterator sit;
            for (sit = inset.begin(); sit != inset.end(); sit++)
                res[current].insert(*sit);
        }
        return res;
    }

private:
    CONCUR_VECTOR<const value_t*> _values;
};