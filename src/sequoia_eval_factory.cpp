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
#include "exceptions.h"
#include "sequoia_eval.h"
#include "sequoia_eval_minmaxcard.h"
#include "sequoia_eval_minmaxcardset.h"
#include "sequoia_eval_bool.h"
#include "sequoia_eval_witness.h"

#include <fstream>
#include <string>

namespace sequoia {

struct is_smaller {
    size_t operator()(size_t a, size_t b) const {
	return a < b;
    }
};
struct is_larger {
    size_t operator()(size_t a, size_t b) const {
	return a > b;
    }
};

typedef MinMaxCardEvaluation<is_smaller> MinCardEvaluation;
typedef MinMaxCardSetEvaluation<is_smaller> MinCardSetEvaluation;

typedef MinMaxCardEvaluation<is_larger> MaxCardEvaluation;
typedef MinMaxCardSetEvaluation<is_larger> MaxCardSetEvaluation;

SequoiaInternalEvaluation *
sequoia_eval_make(const std::string &name,
                  std::vector<const sequoia::ConstantSymbol*> &terminal_symbols,
                  std::vector<const sequoia::UnarySymbol*> &free_symbols) {
    SequoiaInternalEvaluation *res = NULL;
    
    if (name == "Bool") {
        res = new BoolEvaluation();
    } else if (name == "MinCard") {
        res = new MinCardEvaluation();
    } else if (name == "MinCardSet") {
        res = new MinCardSetEvaluation();
    } else if (name == "MaxCard") {
        res = new MaxCardEvaluation();
    } else if (name == "MaxCardSet") {
        res = new MaxCardSetEvaluation();
    } else if (name == "Witness") {
        res = new WitnessEvaluation();
    } else {
        throw sequoia_usage_error("Unknown evaluation");
    }
    res->init(terminal_symbols, free_symbols);
    return res;
}

} // namespace
