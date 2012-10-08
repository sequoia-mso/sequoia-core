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
#ifndef SEQUOIA_LOGIC_STRUCTURE_H
#define	SEQUOIA_LOGIC_STRUCTURE_H

#include "assignment.h"
#include "moves.h"
#include "vocabulary.h"
#include "../config.h"

namespace sequoia {

/**
 * A structure over a vocabulary voc consists of a universe
 * and for each symbol in voc an interpretation of the voc in the structure.
 * For simplicity and efficiency, we only store the vocabulary and the
 * interpretation; the universe is not encluded.  This is because usually
 * we identify existing data structures, such as graphs, with a logical
 * structure and we do not want to duplicate information.
 * 
 * As of 2011-07-15, this class and the functions herein essentially
 * adapt a LabeledGraph and expose it as a structure in the sense of
 * mathematical logic.
 * 
 * TODO: Could use a new Assignment-class that looks up the assignments to the
 * symbols in the graph structure itself.
 * 
 * TODO:  Add Unittests
 * 
 * @param vocabulary The vocabulary for the structure.
 */
class Structure {
public:
    Structure() { }
    Structure(const Vocabulary *vocabulary) : _vocabulary(vocabulary) { }
    virtual ~Structure() {
	delete _vocabulary;
    }
#if 0
    /**
     * @return The interpretation of a given unary symbol in structure.
     */
    const SetMove* interpretation(const UnarySymbol *symbol) const {
        return _interpretation.get(symbol);
    }
    /**
     * @return The interpretation of a given constant symbol in structure.
     */
    const PointMove* interpretation(const ConstantSymbol *symbol) const {
        return _interpretation.get(symbol);
    }
    /**
     * @return The interpretation of a symbol in structure.
     */
    const Move* interpretation(const Symbol *symbol) const {
        return _interpretation.get(symbol);
    }
#endif
    /**
     * @return The structure's vocabulary.
     */
    const Vocabulary* vocabulary() const { return _vocabulary; }
    /**
     * Set the structure's vocabulary.
     * @param vocabulary
     */
    void vocabulary(const Vocabulary *vocabulary) {
        this->_vocabulary = vocabulary;
    }
    virtual std::string toString() const {
        return std::string() + typeid(*this).name() + "(" + _vocabulary->toString() + ")";
    }
#if 0
  protected:
    void interpretation(const Symbol* symbol, const Move* move) {
        _interpretation.set(symbol, move);
    }
#endif
  private:
    const Vocabulary *_vocabulary;
};

} // sequoia

#endif	/* SEQUOIA_STRUCTURE_H */

