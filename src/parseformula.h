/*
 * This file is part of the Sequoia MSO Solver.
 * 
 * Copyright 2012 Fernando Sanchez Villamil, Theoretical Computer Science,
 *                                           RWTH Aachen University
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
 * @author Fernando Sanchez Villamil
 */
#ifndef SEQUOIA_PARSEFORMULA_H
#define SEQUOIA_PARSEFORMULA_H

#include "logic/formula.h"

namespace sequoia {

Formula* parse_string(const Vocabulary* vocabulary, const char* string);
Formula* parse_file(const Vocabulary* vocabulary, const char* file);

}

#endif // SEQUOIA_PARSEFORMULA_H