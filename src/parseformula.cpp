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
#include "common.h"
#include "ms2parser.h"
#include "parseformula.h"
#include "parse_formula_structure.h"

#include <string.h>

using namespace sequoia;
using namespace std;

Formula* convert(PFormula* rawFormula, const Vocabulary* vocabulary);
Formula* convert(PFormula* raw, const Vocabulary* vocabulary,
                 map<unsigned int, const ConstantSymbol*>& variables,
		 map<unsigned int, const UnarySymbol*>& sets) throw (int);
const ConstantSymbol* get_variable (unsigned int id, const char* g, map<unsigned int, const ConstantSymbol*>& variables);
const UnarySymbol* get_set (unsigned int id, const char* name, map<unsigned int, const UnarySymbol*>& sets);
template<typename T> void merge_logical_operator(Formula* formula, T* res);
PFormula* only_atomar_negations(PFormula* rawFormula);
PFormula* only_atomar_negations_(PFormula* rawFormula);
PFormula* negate_pformula(PFormula* formula);
bool is_atomar(PFormula* formula);
PFormula* copy_tree(PFormula* original);
PFormula* insert_arguments(PFormulaPointer* formulaPointer);
void insert_arguments(PFormula* formula,
		      map<int, PVariablePointer*>& variableMapping,
		      map<int, PSetPointer*>& setMapping);
PFormula* flatten(PFormula* formula);
void delete_parser_ast(list<PFormulaPointer*>* list);

namespace sequoia {

Formula* parse_string(const Vocabulary* vocabulary, const char* string) {
  list<PFormulaPointer*>* rawFormula = parse_formula_from_string(string);
  Formula* res = NULL;
  if (rawFormula != NULL) {
    res = convert(rawFormula->back(), vocabulary);
    delete_parser_ast(rawFormula);
  }
  return res;
}

Formula* parse_file(const Vocabulary* vocabulary, const char* string) {
  list<PFormulaPointer*>* rawFormula = parse_formula_from_file(string);
  Formula* res = NULL;
  if (rawFormula != NULL) {
    res = convert(rawFormula->back(), vocabulary);
    delete_parser_ast(rawFormula);
  }
  return res;
}

} // namespace

Formula* convert(PFormula* rawFormula, const Vocabulary *vocabulary) {

  if (rawFormula == NULL) {
    //If the result of the raw parser is failure just keep it like that.
    return NULL;
  }
  if(rawFormula->type != FORMULA_POINTER_OP) {
      std::cerr << "Something went wrong, formula type not as expected" << std::endl;
      return NULL;
  }
  DEBUG(std::cout << "(convert) Input: "; print_raw_tree(rawFormula); std::cout << std::endl;)
  PFormulaPointer* copy = (PFormulaPointer *) copy_tree(rawFormula);
  DEBUG({
      std::cout << "(convert) after copy: " << std::endl;
      print_raw_tree(copy); std::cout << std::endl;
  });

  map<unsigned int, const ConstantSymbol*> variables;
  map<unsigned int, const UnarySymbol*> sets;

  list<struct PPointer*>::iterator it = copy->arguments.begin();
  list<struct PPointer*>::iterator end = copy->arguments.end();
  unsigned int globnest = vocabulary->size();
  const Vocabulary* tmp_vocabulary = vocabulary;
  for (; it != end; ++it) {
      if ((*it)->pointer_type == PPointer::VAR_TYPE) {
	PVariable* vp = ((PVariablePointer*)*it)->variable;
        ConstantSymbol* sym = new ConstantSymbol(str2char(vp->name), globnest++, true);
        variables[vp->id] = sym;
        DEBUG(std::cout << "Adding ConstantSymbol " << sym->identifier() << std::endl;);
        Vocabulary* new_vocabulary = tmp_vocabulary->extend_by(sym);
        if(tmp_vocabulary != vocabulary)
            delete tmp_vocabulary;
        tmp_vocabulary = new_vocabulary;
      } else {
	assert((*it)->pointer_type == PPointer::SET_TYPE);
	PSet* sp =((PSetPointer*)*it)->set;
        const char *ident = str2char(sp->name);
        const UnarySymbol* sym = NULL;
        for(unsigned int i = 0; i < globnest; i++) {
            const Symbol* tmp_s = tmp_vocabulary->symbol(i);
            if (strcmp(ident, tmp_s->identifier()) == 0) {
                sym = (const UnarySymbol*) tmp_s;
                DEBUG({std::cout << "Setting UnarySymbol " << sym->identifier()
                        << " to " << sym->nesting_depth() << std::endl;});
                sets[sp->id] = sym;
                break;
            }
            sym = NULL;
        }
        if (sym != NULL)
            continue;
        sym = new UnarySymbol(ident, globnest++, true);
        DEBUG({std::cout << "Adding UnarySymbol " << sym->identifier()
                        << " to " << sym->nesting_depth() << std::endl;});
        sets[sp->id] = sym;
        Vocabulary* new_vocabulary = tmp_vocabulary->extend_by(sym);
        if(tmp_vocabulary != vocabulary)
            delete tmp_vocabulary;
        tmp_vocabulary = new_vocabulary;
      }
    }
  vocabulary = tmp_vocabulary;
  DEBUG(std::cout << "(convert) flatten the formula..." << std::endl;)
  rawFormula = flatten(copy->formula);
  DEBUG({
      print_raw_tree(rawFormula);
      std::cout << std::endl;
      std::cout << "(convert) only_atomar_negations()" << std::endl;
  })
  rawFormula = only_atomar_negations(rawFormula);
  DEBUG(print_raw_tree(rawFormula); std::cout << std::endl;)

  Formula* res = NULL;
  try {
    res = convert(rawFormula, vocabulary, variables, sets);
  } catch (int e) {
    if (e == 1) {
      cerr << "A negated quantifier was found in the formula. This can not be converted." << std::endl;
    } else {
      cerr << "Unknown error (" << e << ")" << std::endl;
    }
  }

  delete_tree(rawFormula);
  return res;
}

Formula* convert(PFormula* raw, const Vocabulary *vocabulary,
                 map<unsigned int, const ConstantSymbol*>& variables,
		 map<unsigned int, const UnarySymbol*>& sets) throw (int) {

  const ConstantSymbol* variable = NULL;
  const UnarySymbol* set = NULL;
  unsigned int id;
  string name;

  switch (raw->type) {
  case VARIABLE_IN_SET_OP:
    {
      PVariableSetOperation* variableSetOp = (PVariableSetOperation*) raw;
      id = variableSetOp->variable->variable->id;
      name = variableSetOp->variable->variable->name;
      variable = get_variable(id, name.c_str(), variables);
      id = variableSetOp->set->set->id;
      name = variableSetOp->set->set->name;
      set = get_set(id, name.c_str(), sets);
      return new AtomarFormulaMember(vocabulary, variable, set);
    }
  case VARIABLE_NOT_IN_SET_OP:
    {
      PVariableSetOperation* variableSetOp = (PVariableSetOperation*) raw;
      id = variableSetOp->variable->variable->id;
      name = variableSetOp->variable->variable->name;
      variable = get_variable(id, name.c_str(), variables);
      id = variableSetOp->set->set->id;
      name = variableSetOp->set->set->name;
      set = get_set(id, name.c_str(), sets);
      return new NegatedFormula(vocabulary, new AtomarFormulaMember(vocabulary, variable, set));
    }
  case ADJACENT_OP:
    {
      PVariableVariableOperation* variableVariableOp = (PVariableVariableOperation*) raw;
      id = variableVariableOp->left->variable->id;
      name = variableVariableOp->left->variable->name;
      const ConstantSymbol* left = get_variable(id, name.c_str(), variables);
      id = variableVariableOp->right->variable->id;
      name = variableVariableOp->right->variable->name;
      const ConstantSymbol* right = get_variable(id, name.c_str(), variables);
      return new AtomarFormulaAdj(vocabulary, left, right);
    }
  case EQ_VAR_OP:
  {
      PVariableVariableOperation* variableVariableOp = (PVariableVariableOperation*) raw;
      id = variableVariableOp->left->variable->id;
      name = variableVariableOp->left->variable->name;
      const ConstantSymbol* left = get_variable(id, name.c_str(), variables);
      id = variableVariableOp->right->variable->id;
      name = variableVariableOp->right->variable->name;
      const ConstantSymbol* right = get_variable(id, name.c_str(), variables);
      return new AtomarFormulaEquals(vocabulary, left, right);
  }
  case NEG_OP:
    {
      PFormulaOperation* formulaOperation = (PFormulaOperation*) raw;
      const Formula* subFormula = convert(formulaOperation->formula, vocabulary, variables, sets);
      return new NegatedFormula(vocabulary, subFormula);
    }
  case ALL_VARIABLE_OP:
  case EX_VARIABLE_OP:
    {
      PVariableQuant* variableQuant = (PVariableQuant*) raw;
      id = variableQuant->variable->variable->id;
      name = variableQuant->variable->variable->name;
      const ConstantSymbol* variable = new ConstantSymbol(str2char(name),
                                                    vocabulary->size(), true);
      variables[id] = variable;
      Vocabulary *newvoc = vocabulary->extend_by(variable);
      Formula* subFormula = convert(variableQuant->formula, newvoc, variables, sets);
      if (variableQuant->type == EX_VARIABLE_OP)
      	return new ExistObjQFormula(vocabulary, variable, subFormula);
      else
      	return new UnivObjQFormula(vocabulary, variable, subFormula);
    }
  case ALL_SET_OP:
  case EX_SET_OP:
    {
      PSetQuant* setQuant = (PSetQuant*) raw;
      id = setQuant->set->set->id;
      name = setQuant->set->set->name;
      UnarySymbol* set = new UnarySymbol(str2char(name),
                                         vocabulary->size(), true);
      sets[id] = set;
      Vocabulary *newvoc = vocabulary->extend_by(set);
      Formula* subFormula = convert(setQuant->formula, newvoc, variables, sets);
      if (setQuant->type == EX_SET_OP)
      	return new ExistSetQFormula(vocabulary, set, subFormula);
      else
      	return new UnivSetQFormula(vocabulary, set, subFormula);
    }
  case AND_OP:
    {
      PFormulaFormulaOperation* formulaFormulaOperation = (PFormulaFormulaOperation*) raw;
      Formula* f1 = convert(formulaFormulaOperation->left, vocabulary, variables, sets);
      Formula* f2 = convert(formulaFormulaOperation->right, vocabulary, variables, sets);
      ConjBoolCombFormula* res = new ConjBoolCombFormula(vocabulary);
      merge_logical_operator(f1, res);
      merge_logical_operator(f2, res);
      return res;
    }
  case OR_OP:
    {
      PFormulaFormulaOperation* formulaFormulaOperation = (PFormulaFormulaOperation*) raw;
      Formula* f1 = convert(formulaFormulaOperation->left, vocabulary, variables, sets);
      Formula* f2 = convert(formulaFormulaOperation->right, vocabulary, variables, sets);
      DisjBoolCombFormula* res = new DisjBoolCombFormula(vocabulary);
      merge_logical_operator(f1, res);
      merge_logical_operator(f2, res);
      return res;
    }
  case VARIABLE_POINTER_OP:
  case SET_POINTER_OP:
    cerr << "Internal error: Neither VARIABLE_OP nor SET_POINTER_OP should be a possible type at this point." << std::endl;
    return NULL;
  case NEQ_VAR_OP:
    cerr << "'~=' or '!=' can not be converted. " << std::endl; break;
  case EQUIV_OP:
    cerr << "Equivalency operator can not be converted. " << std::endl; break;
  case IMPLIES_OP:
    cerr << "Implication operator can not be converted. " << std::endl; break;
  case FORMULA_POINTER_OP:
    cerr << "Internal error: Found a function pointer while converting the tree, the ast-tree should haven been flattened before conversion.  " << std::endl; break;
  }
  cerr << "convert with type=" << raw->type << std::endl;
  cerr << "Found a symbol in the raw AST that can no be converted to the desired tree structure." << std::endl;
  throw 2;
  return NULL;
}

const ConstantSymbol* get_variable (unsigned int id, const char* name, map<unsigned int, const ConstantSymbol*>& variables) {
  if (variables.count(id) > 0) {
    //map<unsigned int, ConstantSymbol*>::iterator res = variables->find(id);
    //return (*res).second;
    return variables[id];
  } else {
    assert(false); // must not happen, all variables need to be set before
  }
}

const UnarySymbol* get_set (unsigned int id, const char* name, map<unsigned int, const UnarySymbol*>& sets) {
  if (sets.count(id) > 0) {
    //map<unsigned int, UnarySymbol*>::iterator res = sets->find(id);
    //return (*res).second;
    return sets[id];
  } else {
    assert(false); // must not happen, all variables need to be set before
  }
}

template<typename T> void merge_logical_operator(Formula* formula, T* res) {
  T* casted = dynamic_cast<T*> (formula);
  if (casted != NULL) {
      typename T::subformula_iterator it, itend;
      for (it = casted->subformulas_begin(); it != casted->subformulas_end(); it++) {
	  const Formula* addf = *it;
      	res->add_subformula(addf);
      }
    delete casted;
  } else {
    res->add_subformula(formula);
  }
}

PFormula* only_atomar_negations(PFormula* rawFormula) {

  try {
    return only_atomar_negations_(rawFormula);
  } catch (int e){
    if (e == 1) {
      cerr << "Unknown element found in the the ASL while removing negated quants." << std::endl;
    } else if (e == 2) {
      cerr << "Equivalence and Implication operation can not be negated yet." << std::endl;
    }else {
      cerr << "Unknown error while removing negated quants from the ASL." << std::endl;
    }

    return NULL;
  }
}

PFormula* only_atomar_negations_(PFormula* rawFormula) {

  // PFormula* next = rawFormula;

  TYPE type = rawFormula->type;
  switch (type) {
  case NEG_OP:
    {
      PFormulaOperation* negation = (PFormulaOperation*) rawFormula;
      PFormula* res = negate_pformula(negation->formula);
      delete_tree_element(negation);
      if (is_atomar(res))
	return res;
      else
	return only_atomar_negations_(res);
    }
  case AND_OP:
  case OR_OP:
  case IMPLIES_OP:
  case EQUIV_OP:
    {
      PFormulaFormulaOperation* casted =(PFormulaFormulaOperation*)rawFormula;
      casted->left = only_atomar_negations_(casted->left);
      casted->right = only_atomar_negations_(casted->right);
      return casted;
    }
  case ALL_VARIABLE_OP:
  case EX_VARIABLE_OP:
  case ALL_SET_OP:
  case EX_SET_OP:
    {
      PFormulaOperation* casted = (PFormulaOperation*)rawFormula;
      casted->formula = only_atomar_negations_(casted->formula);
      return casted;
    }
  case VARIABLE_NOT_IN_SET_OP:
    {
      PVariableSetOperation* casted = (PVariableSetOperation*)rawFormula;
      casted->type = VARIABLE_IN_SET_OP;
      PFormulaOperation* res = new PFormulaOperation(casted, NEG_OP);
      return res;
      
    }
  case NEQ_VAR_OP:
    {
      PVariableVariableOperation* casted = (PVariableVariableOperation*)rawFormula;
      casted->type = EQ_VAR_OP;
      PFormulaOperation* res = new PFormulaOperation(casted, NEG_OP);
      return res;
    }
  case VARIABLE_IN_SET_OP:
  case VARIABLE_POINTER_OP:
  case SET_POINTER_OP:
  case EQ_VAR_OP:
  case ADJACENT_OP: return rawFormula;
  case FORMULA_POINTER_OP:
    cerr << "There should be no formula pointers in the AST we are converting." << std::endl;
  default: throw 1;
  }
}

PFormula* negate_pformula(PFormula* formula) {
  TYPE type = formula->type;
  switch (type) {
    TYPE negated_type;
  case NEG_OP:
    {
      PFormulaOperation* negation = (PFormulaOperation*) formula;
      PFormula* res = negation->formula;
      delete negation;
      return res;
    }
  case AND_OP:
    negated_type = OR_OP;
  case OR_OP:
    if (type == OR_OP)
      negated_type = AND_OP;

    {
      PFormulaFormulaOperation* casted =(PFormulaFormulaOperation*)formula;
      PFormula* left = negate_pformula(casted->left);
      PFormula* right = negate_pformula(casted->right);
      PFormulaFormulaOperation* res = new PFormulaFormulaOperation(left, right, negated_type);
      delete casted;
      return res;
    }
  case IMPLIES_OP:
  case EQUIV_OP:
    {
      // This operations do not work yet.
      throw 2;
    }
  case ALL_VARIABLE_OP:
    negated_type = EX_VARIABLE_OP;
  case EX_VARIABLE_OP:
    if (type == EX_VARIABLE_OP)
      negated_type = ALL_VARIABLE_OP;

    {
      PVariableQuant* casted = (PVariableQuant*)formula;
      PFormula* newFormula = negate_pformula(casted->formula);
      PVariableQuant* res = new PVariableQuant(new PVariablePointer(new PVariable(casted->variable->variable)), negated_type);
      res->formula = newFormula;
      delete_tree_element(casted);
      return res;
    }
  case ALL_SET_OP:
    if (type == ALL_SET_OP)
      negated_type = EX_SET_OP;
  case EX_SET_OP:
    if (type == EX_SET_OP)
      negated_type = ALL_SET_OP;

    {
      PSetQuant* casted = (PSetQuant*)formula;
      PFormula* newFormula = negate_pformula(casted->formula);
      PSetQuant* res = new PSetQuant(new PSetPointer(new PSet(casted->set->set)), negated_type);
      res->formula = newFormula;
      delete_tree_element(casted);
      return res;
    }
  case VARIABLE_NOT_IN_SET_OP:
    {
      PVariableSetOperation* casted = (PVariableSetOperation*) formula;
      casted->type = VARIABLE_IN_SET_OP;
      return casted;
    }
  case VARIABLE_IN_SET_OP:
  case VARIABLE_POINTER_OP:
  case SET_POINTER_OP:
  case EQ_VAR_OP:
  case NEQ_VAR_OP:
  case ADJACENT_OP:
    {
      PFormulaOperation* res = new PFormulaOperation(formula, NEG_OP);
      return res;
    }
  case FORMULA_POINTER_OP:
    cerr << "There should be no formula pointers in the AST we are converting(negating)." << std::endl;
  default: throw 1;
  }
}

bool is_atomar(PFormula* formula) {
  TYPE type = formula->type;
  switch (type) {
  case NEG_OP:
    {
      PFormulaOperation* casted = (PFormulaOperation*) formula;
      return casted->formula->type != NEG_OP && is_atomar(casted->formula);
    }
  case AND_OP:
  case OR_OP:
  case IMPLIES_OP:
  case EQUIV_OP:
  case ALL_VARIABLE_OP:
  case EX_VARIABLE_OP:
  case ALL_SET_OP:
  case EX_SET_OP:
  case FORMULA_POINTER_OP:
    return false;
  case VARIABLE_IN_SET_OP:
  case VARIABLE_POINTER_OP:
  case SET_POINTER_OP:
  case VARIABLE_NOT_IN_SET_OP:
  case EQ_VAR_OP:
  case NEQ_VAR_OP:
  case ADJACENT_OP:
    return true;
  default: throw 1;
  }
}

PFormula* copy_tree(PFormula* original) {

  TYPE type = original->type;
  switch (type) {
  case VARIABLE_IN_SET_OP:
  case VARIABLE_NOT_IN_SET_OP:
    {
      PVariableSetOperation* casted = (PVariableSetOperation*) original;
      PVariableSetOperation* copy = new PVariableSetOperation(new PVariable(casted->variable->variable),
							      new PSet(casted->set->set), type);
      return copy;
    }
  case EQ_VAR_OP:
  case NEQ_VAR_OP:
  case ADJACENT_OP:
    {
      PVariableVariableOperation* casted = (PVariableVariableOperation*) original;
      PVariableVariableOperation* copy = new PVariableVariableOperation(new PVariable(casted->left->variable),
									new PVariable(casted->right->variable), type);
      return copy;
    }
  case VARIABLE_POINTER_OP:
  case SET_POINTER_OP:
    {
      cerr << "ERROR: Error while copying internal parser tree structure. PVariablePointer ";
      cerr << "and PSetPointer should be copied while copying its parent." << std::endl;
      throw 2;
    }
  case AND_OP:
  case OR_OP:
  case IMPLIES_OP:
  case EQUIV_OP:
    {
      PFormulaFormulaOperation* casted = (PFormulaFormulaOperation*) original;
      PFormula* newLeft = copy_tree(casted->left);
      PFormula* newRight = copy_tree(casted->right);
      PFormulaFormulaOperation* copy = new PFormulaFormulaOperation(newLeft, newRight, type);
      return copy;
    }
  case NEG_OP:
    {
      PFormulaOperation* casted = (PFormulaOperation*) original;
      PFormula* newChild = copy_tree(casted->formula);
      PFormulaOperation* copy = new PFormulaOperation(newChild, type);
      return copy;
    }
  case ALL_VARIABLE_OP:
  case EX_VARIABLE_OP:
    {
      PVariableQuant* casted = (PVariableQuant*) original;
      PVariableQuant* copy = new PVariableQuant(new PVariablePointer(new PVariable(casted->variable->variable)), type);
      copy->formula = copy_tree(casted->formula);
      return copy;
    }
  case ALL_SET_OP:
  case EX_SET_OP:
    {
      PSetQuant* casted = (PSetQuant*) original;
      PSetQuant* copy = new PSetQuant(new PSetPointer(new PSet(casted->set->set)), type);
      copy->formula = copy_tree(casted->formula);
      return copy;
    }
  case FORMULA_POINTER_OP:
    {
      PFormulaPointer* casted = (PFormulaPointer*) original;
      PFormulaPointer* res = new PFormulaPointer(casted);
      return res;
    }
  default: 
    {
      cerr << "ERROR: Error while copying the internal structure tree of the parser. (" << std::endl; 
      cerr << "Aborting." << std::endl;
      throw 1;
    }
  }

  assert (false);
}

PFormula* flatten(PFormula* formula) {

  TYPE type = formula->type;
  switch (type) {
  case VARIABLE_IN_SET_OP:
  case VARIABLE_NOT_IN_SET_OP:
  case EQ_VAR_OP:
  case NEQ_VAR_OP:
  case ADJACENT_OP:
    return formula;
  case VARIABLE_POINTER_OP:
  case SET_POINTER_OP:
    {
      cerr << "ERROR: Error while flattening the internal parser tree structure. PVariablePointer ";
      cerr << "and PSetPointer should be handled while handling the parents." << std::endl;
      throw 2;
    }
  case AND_OP:
  case OR_OP:
  case IMPLIES_OP:
  case EQUIV_OP:
    {
      PFormulaFormulaOperation* casted = (PFormulaFormulaOperation*) formula;
      casted->left = flatten(casted->left);
      casted->right = flatten(casted->right);
      return casted;
    }
  case ALL_VARIABLE_OP:
  case EX_VARIABLE_OP:
  case ALL_SET_OP:
  case EX_SET_OP:
  case NEG_OP:
    {
      PFormulaOperation* casted = (PFormulaOperation*) formula;
      casted->formula = flatten(casted->formula);
      return casted;
    }
  case FORMULA_POINTER_OP:
    {
        //std::cout << "(flatten) called for formula Pointer, which is:" << std::endl;
        //print_raw_tree(formula);
        //std::cout << std::endl;
        //std::cout << "(flatten) first insert arguments into this formula" << std::endl;
        PFormula* converted = insert_arguments((PFormulaPointer*)formula);
        //std::cout << "(flatten) insert arguments done, result:" << std::endl;
        //print_raw_tree(converted);
        //std::cout << std::endl;
        //std::cout << "now flatten this!" << std::endl;
        return flatten(converted);
    }
  default: 
    {
      cerr << "ERROR: Error while copying the internal structure tree of the parser. (" << std::endl; 
      cerr << "Aborting." << std::endl;
      throw 1;
    }
  }

  //The switch should always return or throw something.
  assert (false);
  return formula;
}

PFormula* insert_arguments(PFormulaPointer* formulaPointer) {
  map<int, PVariablePointer*> variableMapping;
  map<int, PSetPointer*> setMapping;

  variableMapping.clear();
  setMapping.clear();

  //std::cout << "insert arguments(formulaPointer)" << std::endl;

  list<PPointer*>::iterator itArguments = formulaPointer->arguments.begin();
  list<PPointer*>::iterator endArguments = formulaPointer->arguments.end();
  list<PPointer*>::iterator itPassed = formulaPointer->passed_arguments.begin();
  //list<PPointer*>::iterator endPassed = formulaPointer->passed_arguments.end();

  for(; itArguments != endArguments; itArguments++, itPassed++) {
    PPointer* current = (*itArguments);
    if (current->pointer_type == PPointer::VAR_TYPE) {
      variableMapping[((PVariablePointer*)current)->variable->id] = (PVariablePointer*)(*itPassed);
    } else if (current->pointer_type == PPointer::SET_TYPE) {
      setMapping[((PSetPointer*)current)->set->id] = (PSetPointer*)(*itPassed);
    }
  }

  //std::cout << "insert arguments: first copy formula " << formulaPointer->formula << std::endl;
  PFormula* copy = copy_tree(formulaPointer->formula);
  //std::cout << "now recursively insert arguments into the copied version" << std::endl;
  insert_arguments(copy, variableMapping, setMapping);
  //std::cout << "insert arguments done" << std::endl;
  //print_raw_tree(copy); std::cout << std::endl;
  //std::cout << "now delete old pointer " << std::endl;

  delete_tree_element(formulaPointer);

  return copy;
}

void insert_arguments(PFormula* formula,
		      map<int, PVariablePointer*>& variableMapping,
		      map<int, PSetPointer*>& setMapping) {

  TYPE type = formula->type;
  //std::cout << "insert_arguments(formula) with type: " << type << std::endl;
  switch (type) {
  case VARIABLE_IN_SET_OP:
  case VARIABLE_NOT_IN_SET_OP:
    {
      PVariableSetOperation* casted = (PVariableSetOperation*)formula;
      int id = casted->variable->variable->id;
      if (variableMapping.count(id) > 0) {
	delete casted->variable->variable;
	casted->variable->variable = new PVariable(variableMapping[id]->variable);
      }
      id = casted->set->set->id;
      if (setMapping.count(id) > 0) {
	delete casted->set->set;
	casted->set->set = new PSet(setMapping[id]->set);
      }
      break;
    }
  case EQ_VAR_OP:
  case NEQ_VAR_OP:
  case ADJACENT_OP:
    {
      PVariableVariableOperation* casted = (PVariableVariableOperation*)formula;
      int id = casted->left->variable->id;
      if (variableMapping.count(id) > 0) {
	delete casted->left->variable;
	casted->left->variable = new PVariable(variableMapping[id]->variable);
      }
      id = casted->right->variable->id;
      if (variableMapping.count(id) > 0) {
	delete casted->right->variable;
	casted->right->variable = new PVariable(variableMapping[id]->variable);
      }
      break;
    }
  case VARIABLE_POINTER_OP:
  case SET_POINTER_OP:
    {
      throw 300;
    }
  case AND_OP:
  case OR_OP:
  case IMPLIES_OP:
  case EQUIV_OP:
    {
      PFormulaFormulaOperation* casted = (PFormulaFormulaOperation*)formula;
      insert_arguments(casted->left, variableMapping, setMapping);
      insert_arguments(casted->right, variableMapping, setMapping);
      break;
    }
  case ALL_VARIABLE_OP:
  case EX_VARIABLE_OP:
    {
      PVariableQuant* casted = (PVariableQuant*)formula;
      int id = casted->variable->variable->id;
      if (variableMapping.count(id) > 0) {
	delete casted->variable->variable;
	casted->variable->variable = new PVariable(variableMapping[id]->variable);
      }
      insert_arguments(casted->formula, variableMapping, setMapping);
      break;
    }
  case ALL_SET_OP:
  case EX_SET_OP:
    {
      PSetQuant* casted = (PSetQuant*)formula;
      int id = casted->set->set->id;
      if (setMapping.count(id) > 0) {
	delete casted->set->set;
	casted->set->set = new PSet(setMapping[id]->set);
      }
      insert_arguments(casted->formula, variableMapping, setMapping);
      break;
    }
  case NEG_OP:
    {
      PFormulaOperation* casted = (PFormulaOperation*)formula;
      insert_arguments(casted->formula, variableMapping, setMapping);
      break;
    }
  case FORMULA_POINTER_OP:
    {
      PFormulaPointer* casted = (PFormulaPointer*)formula;
      list<PPointer*> copy(casted->passed_arguments);
      casted->passed_arguments.clear();

      list<PPointer*>::iterator it = copy.begin();
      list<PPointer*>::iterator end = copy.end();
      for(;it != end; it++) {
	int id;
	if ((*it)->pointer_type == PPointer::VAR_TYPE) {
	  PVariablePointer* varP = (PVariablePointer*)(*it);
	  id = varP->variable->id;
	  if (variableMapping.count(id) == 0) {
	    casted->passed_arguments.push_back(*it);
	  } else {
	    delete varP->variable;
	    delete varP;
	    casted->passed_arguments.push_back(new PVariablePointer(variableMapping[id]));
	  }
	} else {
	  assert((*it)->pointer_type == PPointer::SET_TYPE);
	  PSetPointer* setP = (PSetPointer*)(*it);
	  id = setP->set->id;
	  if (setMapping.count(id) == 0) {
	    casted->passed_arguments.push_back(*it);
	  } else {
	    delete setP->set;
	    delete setP;
	    casted->passed_arguments.push_back(new PSetPointer(setMapping[id]));
	  }
	}
      }
      break;
    }
  default: 
    {
      cerr << "ERROR: Error while copying the internal structure tree of the parser." << std::endl; 
      cerr << "Aborting." << std::endl;
      throw 1;
    }
  }
}

void delete_parser_ast(list<PFormulaPointer*>* formulaList) {

    // XXX avoid crashes by not freeing anything.
    return;

  list<PFormulaPointer*>::iterator it = formulaList->begin();
  list<PFormulaPointer*>::iterator end = formulaList->end();
  for (;it != end; it++) {
    delete_tree((*it)->formula);
    delete_tree_element((*it));
  }
  
  delete formulaList;
}
