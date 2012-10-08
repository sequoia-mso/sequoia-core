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
#ifndef SEQUOIA_MS2PARSER_H
#define SEQUOIA_MS2PARSER_H

#include "parse_formula_structure.h"

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <list>
#include <sstream>

std::list<PFormulaPointer*>* parse_formula_from_file(const char* file);
std::list<PFormulaPointer*>* parse_formula_from_string(const char* ms2string);

void delete_tree(PFormula*);
void delete_tree_element(PFormula*);

#endif // SEQUOIA_MS2PARSER_H