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
#ifndef SEQUOIA_ATOMAR_GAME_FACTORY_H
#define SEQUOIA_ATOMAR_GAME_FACTORY_H

#include "game.h"
#include "logic/formula_visitor.h"

namespace sequoia {

class AtomarGameFactory : public FormulaVisitor {
public:
    AtomarGameFactory(const Assignment_f* alpha) :
        _alpha(alpha) {
            DEBUG(_level = 0);
    }
    void visit(const UnivSetQFormula* f);
    void visit(const ExistSetQFormula* f);
    void visit(const UnivObjQFormula* f);
    void visit(const ExistObjQFormula* f);
    void visit(const ConjBoolCombFormula* f);
    void visit(const DisjBoolCombFormula* f);

    void visit(const AtomarFormulaMember* f);
    void visit(const AtomarFormulaAdj* f);
    void visit(const AtomarFormulaEquals* f);
    void visit(const NegatedFormula* f);

    const MCGame_f* get() { return _game; }
    DEBUG(void level(int level) { _level = level; })
private:
    const Assignment_f* _alpha;
    const MCGame_f* _game;
    DEBUG(int _level;)
};

} // namespace

#endif // SEQUOIA_ATOMAR_GAME_FACTORY_H