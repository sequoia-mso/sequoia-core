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
#include "vocabulary.h"
#include "../config.h"

namespace sequoia {

bool Vocabulary::extends(const Vocabulary* other) const {
    return other->reduces(this);
}

bool Vocabulary::reduces(const Vocabulary* other) const {
    unsigned int s = size();
    if (other->size() < s) return false;
    for(unsigned int i = 0; i < s; ++i)
        if (_symbols[i] != other->_symbols[i])
            return false;
    return true;
}

std::string Vocabulary::toString() const {
    std::stringstream res;
    res << "Vocabulary[Symbols=";
    for (unsigned int i = 0; i < size(); ++i) {
        res << (i > 0 ? "," : "") << _symbols[i]->identifier();
    }
    res << "|Const=";
    for (unsigned int i = 0; i < number_of_constant_symbols(); ++i)
        res << (i > 0 ? "," : "") <<
            _constant_symbols[i]->identifier();
    res << "|Unary=";
    for (unsigned int i = 0; i < number_of_unary_symbols(); ++i)
        res << (i > 0 ? "," : "") <<
            _unary_symbols[i]->identifier();
    res << "]";
    return res.str();
}

} // sequoia
