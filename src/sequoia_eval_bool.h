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
#ifndef SEQUOIA_EVAL_BOOL_H
#define SEQUOIA_EVAL_BOOL_H

#include "sequoia_eval.h"
#include <string>

namespace sequoia {

/**
 * The BoolEvaluation does not evaluate at all but just distinguished
 * between solutions and non-solutions.  This evaluation is suitable for
 * decision problems.
 * Multiplication:  result is the logical and.
 * Addition: result is the logical or.
 */
class BoolEvaluation : public SequoiaInternalEvaluation {
public:
    const void * elem(const sequoia::Bag *bag,
                      const sequoia::Assignment *in) {
        return NULL;
    }

    const void * mult(const void *e1, const void *e2, const void *intersect) {
        return NULL;
    }

    const void * add(const void *e1, const void *e2) {
        return NULL;
    }

    void output_solution(std::ostream &outs, const void *) const {
        outs << "TRUE";
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

#endif // SEQUOIA_EVAL_BOOL_H
