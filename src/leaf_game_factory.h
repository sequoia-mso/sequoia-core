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
#ifndef SEQUOIA_LEAF_GAME_FACTORY_H
#define SEQUOIA_LEAF_GAME_FACTORY_H

#include "logic/formula_visitor.h"
#include "game.h"
#include "game_bool_comb.h"
#include "game_q.h"
#include "moves_pool.h"

namespace sequoia {

class SequoiaSolver;

class LeafGameFactory : public FormulaVisitor {
public:
    LeafGameFactory(const Assignment_f* alpha) :
        _alpha(alpha) {
        DEBUG(_level = 0);
    }
    
    void visit(const UnivSetQFormula* f) {
	visit_impl<UnivSetQFormula, SetMove, UnivSetQUndetGame>(f);
    }
    void visit(const ExistSetQFormula* f) {
	visit_impl<ExistSetQFormula, SetMove, ExistSetQUndetGame>(f);
    }
    void visit(const UnivObjQFormula* f) {
	visit_impl<UnivObjQFormula, PointMove, UnivObjQUndetGame>(f);
    }
    void visit(const ExistObjQFormula* f) {
	visit_impl<ExistObjQFormula, PointMove, ExistObjQUndetGame>(f);
    }
    void visit(const ConjBoolCombFormula* f) {
	visit_impl<ConjBoolCombFormula, Formula, ConjBoolCombUndetGame>(f);
    }
    void visit(const DisjBoolCombFormula* f) {
	visit_impl<DisjBoolCombFormula, Formula, DisjBoolCombUndetGame>(f);
    }

    void visit(const AtomarFormulaMember* f) { do_atomar(f); }
    void visit(const AtomarFormulaAdj* f) { do_atomar(f); }
    void visit(const AtomarFormulaEquals* f) { do_atomar(f); }
    void visit(const NegatedFormula* f) { do_atomar(f); }

    const MCGame_f* get() { return _game; }
    DEBUG(void level(int level) { _level = level; })
  protected:
    void do_atomar(const Formula* f);

    template <typename TFormula, typename TIndex, typename TReturnGame>
    void visit_impl(const TFormula* f);
private:
    const SetMove* prepare_index(const SetQFormula* f,
                                 const Formula* subf,
                                 boost::scoped_ptr<const Assignment_f> &alpha) {
	const SetMove* smove = moves_pool.pool(new SetMove(f->variable()));
        alpha.reset(AssignmentFlyFactory::make(new SetAssignment(alpha.get(), f->variable(), smove)));
	return smove;
    }
    const PointMove* prepare_index(const ObjQFormula* f,
                                   const Formula* subf,
                                   boost::scoped_ptr<const Assignment_f> &alpha) {
        alpha.reset(AssignmentFlyFactory::make(new ObjAssignment(alpha.get(), f->variable(), NULL)));
	return NULL;
    }
    const Formula* prepare_index(const ConjBoolCombFormula* f,
                                 const Formula* subf,
                                 boost::scoped_ptr<const Assignment_f> &alpha) {
	return subf;
    }
    const Formula* prepare_index(const DisjBoolCombFormula* f,
                                 const Formula* subf,
                                 boost::scoped_ptr<const Assignment_f> &alpha) {
	return subf;
    }

    const Assignment_f* _alpha;
    const MCGame_f* _game;
    DEBUG(int _level;)
};

} // namespace

#include "leaf_game_factory.hpp"

#endif // SEQUOIA_LEAF_GAME_FACTORY_H