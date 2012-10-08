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
#ifndef SEQUOIA_LOGIC_FORMULA_VISITOR_H
#define SEQUOIA_LOGIC_FORMULA_VISITOR_H

#include "../config.h"

namespace sequoia {

// declare elements to make them available to the visitor
class UnivSetQFormula;
class ExistSetQFormula;
class UnivObjQFormula;
class ExistObjQFormula;
class ConjBoolCombFormula;
class DisjBoolCombFormula;
class AtomarFormulaMember;
class AtomarFormulaAdj;
class AtomarFormulaEquals;
class NegatedFormula;

class FormulaVisitor {
public:
    virtual void visit(const UnivSetQFormula* f) = 0;
    virtual void visit(const ExistSetQFormula* f) = 0;
    virtual void visit(const UnivObjQFormula* f) = 0;
    virtual void visit(const ExistObjQFormula* f) = 0;
    virtual void visit(const ConjBoolCombFormula* f) = 0;
    virtual void visit(const DisjBoolCombFormula* f) = 0;

    virtual void visit(const AtomarFormulaMember* f) = 0;
    virtual void visit(const AtomarFormulaAdj* f) = 0;
    virtual void visit(const AtomarFormulaEquals* f) = 0;
    virtual void visit(const NegatedFormula* f) = 0;
};

} // namespace

#endif // SEQUOIA_LOGIC_FORMULA_VISITOR_H