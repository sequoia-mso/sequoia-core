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
#ifndef SEQUOIA_EVALUATION_H
#define SEQUOIA_EVALUATION_H

#include <set>
#include <string>
#include <vector>

namespace sequoia {

/**
 * An evaluation from the ring of solutions into a semiring of choice.
 * This class maps (partial) solutions, i.e., assignments
 * to the free variables of a formula, into a desired semiring.
 *
 * Since arbitrary ring elements are allowed, all operations take
 * and return void pointers that can hence represent arbitrary values.
 */
class Evaluation {
public:
    Evaluation() { }
    virtual ~Evaluation() { }

    /**
     * This is used to initialize the evaluation after loading them
     * via dlopen().
     * 
     * @param free_symbols List of the free variables of the formula
     *                     that the evaluation can access.
     * @param argc the number of arguments in argv to follow
     * @param argv the command line arguments given to the evaluation
     */
    virtual void init(std::vector<const char *> &free_symbols,
              int argc, const char **argv) = 0;

    /**
     * The actual homomorphism:  Maps elements from the input structure
     * into elements of the target ring.  The current bag contains the
     * terminals in order 0, ..., w-1;  The assignment specifies which of
     * the (interpretations of the) terminal symbols are contained.
     * @param in The current interpretation of the free variables.
     * @return The obtained ring element.
     */
    virtual const void* elem(std::vector<std::set<int> > &in) = 0;
    
    /**
     * The multiplication \f$e_1 \times e_2\f$ in the target semiring:
     * Given two ring elements \f$e_1\f$ and \f$e_2\f$, return \f$e_1 \times e_2\f$.
     * Note that this operation actually needs to represent the result
     * under the union, i.e., the original sets are in general not disjoint.
     * Here, intersect represents the ring element obtained from the intersection:
     * If mult() implements the homomorphism \f$h\f$, then
     * \f$intersect = h(h^{-1}(e_1) \cap h^{-1}(e_2))\f$.
     * It is therefore important that mult() returns the value of
     * \f$h(h^{-1}(e_1) \cup h^{-1}(e_2))\f$.
     * @param e1 The first ring element \f$e_1\f$.
     * @param e2 The second ring element \f$e_2\f$.
     * @param intersect The intersection defined as above.
     * @return The obtained ring element \f$h(h^{-1}(e_1) \cup h^{-1}(e_2))\f$.
     */
    virtual const void* mult(const void *e1, const void *e2,
                             const void *intersect) = 0;

    /**
     * The addition in the target semiring:
     * Given two ring elements \f$e_1\f$ and \f$e_2\f$, return \f$e_1 + e_2\f$.
     * @param e1 The first ring element \f$e_1\f$.
     * @param e2 The second ring element \f$e_2\f$.
     * @return The obtained ring element \f$h(h^{-1}(e_1) \mathrel{\bar\cup} h^{-1}(e_2))\f$.
     */
    virtual const void* add(const void *e1, const void *e2) = 0;

    /**
     * Convert the ring element $e$ into a (human-readable) representation.
     * @param e The ring element to be printed.
     */
    virtual void output_solution(std::ostream &outs, const void *e) const = 0;

    /**
     * Return true iff it is possible to return a vertex set of solutions.
     */
    virtual bool returns_vertex_sets() const = 0;

    typedef std::vector<std::set<int> > vertex_sets_t;
    /**
     * Convert to a solution.
     * Prerequisite: returns_vertex_sets() == true
     * @param e
     * @return 
     */
    virtual vertex_sets_t
    convert_to_vertex_sets(const void *e) const = 0;
};

} // namespace
    
#endif // SEQUOIA_EVALUATION_H