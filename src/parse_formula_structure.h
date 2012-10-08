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
#ifndef SEQUOIA_PARSE_FORMULA_STRUCTURE_H
#define SEQUOIA_PARSE_FORMULA_STRUCTURE_H

#include <assert.h>
#include <list>
#include <string>

enum TYPE {
  VARIABLE_IN_SET_OP,       // 0
  VARIABLE_NOT_IN_SET_OP,   // 1
  EQ_VAR_OP,                // 2
  NEQ_VAR_OP,               // 3
  ADJACENT_OP,              // 4
  AND_OP,                   // 5
  OR_OP,                    // 6
  IMPLIES_OP,               // 7
  EQUIV_OP,                 // 8
  NEG_OP,                   // 9
  VARIABLE_POINTER_OP,      // 10
  SET_POINTER_OP,           // 11
  ALL_VARIABLE_OP,          // 12
  EX_VARIABLE_OP,           // 13
  ALL_SET_OP,               // 14
  EX_SET_OP,                // 15
  FORMULA_POINTER_OP        // 16
};

enum QUANTIFIER {
  ALL_QUANT,
  EX_QUANT,
  NO_QUANT
};

/*
 * Structure declarations.
 */

struct PVariable {
  std::string name;
  QUANTIFIER quantifier;
  unsigned int id;
  bool isFree;

  int line;
  int column;

  PVariable(char* name, QUANTIFIER quantifier, unsigned int id,
	   int line, int column) {
    this->name = name;
    this->quantifier = quantifier;
    this->id = id;
    this->isFree = false;

    this->line = line;
    this->column = column;
  }

  PVariable(PVariable* other) {
    assert (other != NULL);
    this->name = other->name;
    this->quantifier = other->quantifier;
    this->id = other->id;
    this->isFree = other->isFree;

    this->line = other->line;
    this->column = other->column;
  }

  ~PVariable() {
      //std::cout << "PVariable delete: " << this << "(" << this->name << ":" << this->id << ")" << std::endl;
  }
};

struct PFreeVariable : PVariable {
  PFreeVariable(char* name, unsigned int id, int line, int column)
    : PVariable(name, NO_QUANT, id, line, column) {
    this->isFree = true;
  }

  PFreeVariable(PFreeVariable* other) : PVariable(other) {
    this->isFree = true;
  }
};

struct PSet {
  std::string name;
  QUANTIFIER quantifier;
  unsigned int id;
  bool isFree;
  
  int line;
  int column;

  PSet(char* name, QUANTIFIER quantifier, unsigned int id,
      int line, int column) {
    this->name = name;
    this->quantifier = quantifier;
    this->id = id;
    this->isFree = false;

    this->line = line;
    this->column = column;
  }

  PSet(PSet* other) {
    this->name = other->name;
    this->quantifier = other->quantifier;
    this->id = other->id;
    this->isFree = other->isFree;

    this->line = other->line;
    this->column = other->column;
  }

  ~PSet() {
  }
};

struct PFreeSet : PSet{
  PFreeSet(char* name, unsigned int id, int line, int column)
    : PSet(name, NO_QUANT, id, line, column) {
    this->isFree = true;
  }

  PFreeSet(PFreeSet* other) : PSet(other) {
    this->isFree = true;
  }
};

struct PFormula {
  TYPE type;
  
  PFormula(TYPE type) {
    this->type = type;
    
    // If the type is VARIABLE_OP then it will be erased by its parent.
    if (type != VARIABLE_POINTER_OP && type != SET_POINTER_OP) {
      //garbageCollector->insert(this);
    }
  }
};

PFormula* copy_tree(PFormula* original);

struct PPointer : PFormula {
  enum POINTER_TYPE {
    VAR_TYPE,
    SET_TYPE
  };

  POINTER_TYPE pointer_type;

  PPointer (POINTER_TYPE pointer_type, TYPE type) : PFormula(type) {
    this->pointer_type = pointer_type;
  }
};

struct PVariablePointer : PPointer {
  PVariable* variable;
  
  PVariablePointer(PVariable* variable) : PPointer(VAR_TYPE, VARIABLE_POINTER_OP), variable(NULL) {
    this->variable = variable;
  }

  //Copy constructor
  PVariablePointer(PVariablePointer* other) : PPointer(VAR_TYPE, VARIABLE_POINTER_OP), variable(NULL) {
    this->variable = new PVariable(other->variable);
  }

  ~PVariablePointer() {
      // XXX mem leak here, but otherwise serious problems with flatten() in pareformula()
  }
};

struct PSetPointer : PPointer {
  PSet* set;
  PSetPointer (PSet* set) : PPointer(SET_TYPE, SET_POINTER_OP), set(NULL) {
    this->set = set;
  }

  //Copy constructor
  PSetPointer(PSetPointer* other) : PPointer(SET_TYPE, SET_POINTER_OP), set(NULL) {
    this->set = new PSet(other->set);
  }

  ~PSetPointer() {
      // XXX mem leak here, but otherwise serious problems with flatten() in pareformula()
  }
};

struct PFormulaPointer : PFormula {
  std::list<PPointer*> arguments;
  std::list<PPointer*> passed_arguments;

  std::string name;
  PFormula* formula;

  PFormulaPointer() : PFormula(FORMULA_POINTER_OP) {
    this->formula = NULL;
  }

  PFormulaPointer(PFormulaPointer* other) : PFormula(FORMULA_POINTER_OP) {
    std::list<struct PPointer*>:: iterator it = other->arguments.begin();
    std::list<struct PPointer*>:: iterator end = other->arguments.end();
    for (;it != end; it++) {
      if ((*it)->pointer_type == PPointer::VAR_TYPE) {
	PVariablePointer* newPointer = new PVariablePointer(new PVariable(((PVariablePointer*)*it)->variable));
	this->arguments.push_back(newPointer);
      } else {
	assert((*it)->pointer_type == PPointer::SET_TYPE);
	PSetPointer* newPointer = new PSetPointer(new PSet(((PSetPointer*)*it)->set));
	this->arguments.push_back(newPointer);
      }
    }

    std::list<struct PPointer*>:: iterator itP = other->passed_arguments.begin();
    std::list<struct PPointer*>:: iterator endP = other->passed_arguments.end();
    for (;itP != endP; itP++) {
      if ((*itP)->pointer_type == PPointer::VAR_TYPE) {
	PVariablePointer* newPointer = new PVariablePointer(new PVariable(((PVariablePointer*)*itP)->variable));
	this->passed_arguments.push_back(newPointer);
      } else {
	assert((*itP)->pointer_type == PPointer::SET_TYPE);
	PSetPointer* newPointer = new PSetPointer(new PSet(((PSetPointer*)*itP)->set));
	this->passed_arguments.push_back(newPointer);
      }
    }

    this->name = other->name;
    this->formula = copy_tree(other->formula);
  }

  void push_variable(PVariable* variable) {
    // Here we got to push at the front because the LALR(1) parser
    // parses the last argument first.
    this->arguments.push_front(new PVariablePointer(variable));
  }

  void push_set(PSet* set) {
    // Here we got to push at the front because the LALR(1) parser
    // parses the last argument first.
    this->arguments.push_front(new PSetPointer(set));
  }
  
  ~PFormulaPointer() {
  }
};

struct PFormulaOperation : PFormula {
  PFormula* formula;

  PFormulaOperation(PFormula* formula, TYPE type) : PFormula (type) {
    this->type = type;
    this->formula = formula;
  }

  ~PFormulaOperation() {
  }
};

struct PQuant : PFormulaOperation{
  //PFormula* formula;

  PQuant(TYPE type) : PFormulaOperation(NULL, type) {
    //Just passing the type up.
  }
};

struct PVariableQuant : PQuant {
  PVariablePointer* variable;

  PVariableQuant(PVariablePointer* variable, TYPE type) : PQuant(type) {
    this->variable = variable;
  }

  ~PVariableQuant() {
    // Notice that this destructor just deletes the variable pointer,
    // not the actual variable structure containing all the information
    // related to the variable.
    //if (variable != NULL)
      //delete variable;
  }
};

struct PSetQuant : PQuant {
  PSetPointer* set;

  PSetQuant(PSetPointer* set, TYPE type) : PQuant(type) {
    this->set = set;
  }

  ~PSetQuant() {
    // Notice that this destructor just deletes the set pointer,
    // not the actual set structure containing all the information
    // related to the set.
    //if (set != NULL)
      //delete set;
  }
};

struct PVariableSetOperation : PFormula {
  PVariablePointer* variable;
  PSetPointer* set;

  PVariableSetOperation(PVariable* variable, PSet* set, TYPE type)
    : PFormula(type) {

    this->variable = new PVariablePointer(variable);
    this->set = new PSetPointer(set);
  }

  ~PVariableSetOperation() {
#if 0
    if (variable != NULL)
      delete variable;

    if (set != NULL)
      delete set;
#endif
  }
};

struct PVariableVariableOperation : PFormula {
  PVariablePointer* left;
  PVariablePointer* right;

  PVariableVariableOperation(PVariable* left, PVariable* right, TYPE type)
    : PFormula(type) {

    this->left = new PVariablePointer(left);
    this->right = new PVariablePointer(right);
  }

  ~PVariableVariableOperation() {
#if 0
    if (left != NULL)
      delete left;

    if (right != NULL)
      delete right;
#endif
  }
};

struct PFormulaFormulaOperation : PFormula{
  PFormula* left;
  PFormula* right;

  PFormulaFormulaOperation(PFormula* left, PFormula* right, TYPE type)
    : PFormula(type) {

    type = type;
    this->left = left;
    this->right = right;
  }

  ~PFormulaFormulaOperation() {
  }
};

void print_raw_tree(PFormula*);

#endif // SEQUOIA_PARSE_FORMULA_STRUCTURE_H