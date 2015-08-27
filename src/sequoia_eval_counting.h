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
 * @author Peter Rossmanith
 */
#ifndef SEQUOIA_EVAL_COUNTING_H
#define SEQUOIA_EVAL_COUNTING_H

#include "sequoia_eval.h"

namespace sequoia {

/**
 * The CountingEvaluation finds the number of solutions
 * for the first free set variable.
 * Multiplication:  result is the product of (partial) solutions.
 * Addition: result is the sum of the (partial) solutions.
 */
class CountingEvaluation : public SequoiaInternalEvaluation {
public:
    const void * elem(const sequoia::Bag *bag,
                      const sequoia::Assignment *alpha) {
        assert(_free_symbols.size() > 0);
        //const sequoia::SetMove *smove = alpha->get(_free_symbols[0]);
        unsigned long out = 1;
        return (void *) out;
    }

    const void * mult(const void *e1, const void *e2, const void *intersect) {
        unsigned long i1 = (unsigned long) e1;
        unsigned long i2 = (unsigned long) e2;
        unsigned long is = (unsigned long) intersect;
        return (void *) (i1 * i2);
    }

    const void * add(const void *e1, const void *e2) {
        unsigned long i1 = (unsigned long) e1;
        unsigned long i2 = (unsigned long) e2;
	return (void *) (i1 + i2);
    }

    void output_solution(std::ostream &outs, const void *e) const {
        outs << ((unsigned long) e);
    }

    bool returns_vertex_sets() const {
        return false;
    }

    vertex_sets_t
    convert_to_vertex_sets(const void *e) const {
        return vertex_sets_t();
    }

};

} // namespace

#endif // SEQUOIA_EVAL_COUNTING_H
