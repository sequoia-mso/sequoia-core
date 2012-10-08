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
#ifndef SEQUOIA_EVAL_H
#define SEQUOIA_EVAL_H

#include "evaluation.h"
#include "logic/symbols.h"
#include "logic/assignment.h"
#include "structures/bag.h"

#include <fstream>
#include <string>

namespace sequoia {

/**
 * An evaluation from the ring of solutions into a semiring of choice.
 * This class maps (partial) solutions, i.e., assignments
 * to the free variables of a formula, into a desired semiring.
 *
 * Since arbitrary ring elements are allowed, all operations take
 * and return void pointers that can hence represent arbitrary values.
 * @param terminal_symbols The set of terminal symbols (constants that mark
 *                         particular vertices).
 * @param free_symbols List of the free variables that the evaluation can
 *                     access.
 */
class SequoiaInternalEvaluation : public Evaluation {
public:
    /* Here come our own replacement calls that are more efficient */
    virtual void init(std::vector<const sequoia::ConstantSymbol*> &terminal_symbols,
                      std::vector<const sequoia::UnarySymbol*> &free_symbols) {
        _free_symbols = free_symbols;
        _terminal_symbols = terminal_symbols;
    }
    virtual const void* elem(const sequoia::Bag *bag,
                             const sequoia::Assignment *assignment) = 0;

    // never to be called in the internal context
    void init(std::vector<const char *> &free_symbols,
              int argc, const char **argv) {
        assert(false);
    }
    // never to be called in the internal context
    const void* elem(std::vector<std::set<int> > &in) {
        assert(false);
        return NULL;
    }

    // for debug output
    std::string toString(const void *e) {
        std::stringstream s;
        output_solution(s, e);
        return s.str();
    }

protected:
    std::vector<const sequoia::ConstantSymbol*> _terminal_symbols;
    std::vector<const sequoia::UnarySymbol*> _free_symbols;
};

} // namespace
    
#endif // SEQUOIA_EVAL_H
