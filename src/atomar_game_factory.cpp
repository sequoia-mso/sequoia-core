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
#include "atomar_game_factory.h"
#include "game_atomar.h"
#include "game_determined.h"

#include <assert.h>

namespace sequoia {

void AtomarGameFactory::visit(const UnivSetQFormula* f) { assert(false); }
void AtomarGameFactory::visit(const ExistSetQFormula* f) { assert(false); }
void AtomarGameFactory::visit(const UnivObjQFormula* f) { assert(false); }
void AtomarGameFactory::visit(const ExistObjQFormula* f) { assert(false); }
void AtomarGameFactory::visit(const ConjBoolCombFormula * f) { assert(false); }
void AtomarGameFactory::visit(const DisjBoolCombFormula * f) { assert(false); }

void AtomarGameFactory::visit(const AtomarFormulaMember * f) {
    assert(f != NULL);
    DEBUG({
        tab_prefix(_level) << "Evaluating for Formula: "
                << f->toString() << std::endl;
    });
    const ConstantSymbol* x = f->constant_symbol();
    const UnarySymbol* U = f->unary_symbol();
    assert(x != NULL && U != NULL);

    const PointMove *pmove = _alpha->get()->get(x);
    const SetMove *smove = _alpha->get()->get(U);
    assert(smove != NULL);

    if (pmove == NULL) {
        AtomarUndetGame* returngame = new AtomarUndetGame(f);
        DEBUG(returngame->level(_level));
        _game = MCGameFlyFactory::make(returngame);
        return;
    }

    assert(pmove->nesting_depth() != smove->nesting_depth());
    if (pmove->nesting_depth() > smove->nesting_depth()) {
        if (pmove->test_label(smove->symbol())) {
            _game = determined.get(MCGame::VERIFIER);
        } else {
            _game = determined.get(MCGame::FALSIFIER);
        }
    } else {
        if (smove->test(pmove->symbol())) {
            _game = determined.get(MCGame::VERIFIER);
        } else {
            _game = determined.get(MCGame::FALSIFIER);
        }
    }
}

void AtomarGameFactory::visit(const AtomarFormulaAdj * f) {
    assert(f != NULL);
    DEBUG({
        tab_prefix(_level) << "Evaluating for Formula: "
                << f->toString() << std::endl;
    });
    const ConstantSymbol* x = f->x();
    const ConstantSymbol* y = f->y();
    assert(x != NULL && y != NULL);

    const PointMove *xmove = _alpha->get()->get(x);
    const PointMove *ymove = _alpha->get()->get(y);

    // both are not set, so game remains undetermined
    if (xmove == NULL && ymove == NULL) {
        AtomarUndetGame* returngame = new AtomarUndetGame(f);
        DEBUG(returngame->level(_level));
        _game = MCGameFlyFactory::make(returngame);;
        return;
    }
    // if both are set, we lookup the adjacency in the vertex with
    // the higher nesting depth
    if (xmove != NULL && ymove != NULL) {
        if (xmove->nesting_depth() > ymove->nesting_depth()) {
            if (xmove->test_edge(ymove->nesting_depth())) {
                _game = determined.get(MCGame::VERIFIER);
                return;
            } else {
                _game = determined.get(MCGame::FALSIFIER);
                return;
            }
        } else {
            if (ymove->test_edge(xmove->nesting_depth())) {
                _game = determined.get(MCGame::VERIFIER);
                return;
            } else {
                _game = determined.get(MCGame::FALSIFIER);
                return;
            }
        }
    }
    assert(xmove == NULL || ymove == NULL);

    // The bag is a separator.  Thus if none of them is a terminal, they are
    // not adjacent.
    if(xmove != NULL && !xmove->terminal()) {
        _game = determined.get(MCGame::FALSIFIER);
        return;
    }
    if(ymove != NULL && !ymove->terminal()) {
        _game = determined.get(MCGame::FALSIFIER);
        return;
    }
    AtomarUndetGame* returngame = new AtomarUndetGame(f);
    DEBUG(returngame->level(_level));
    _game = MCGameFlyFactory::make(returngame);
}

void AtomarGameFactory::visit(const AtomarFormulaEquals* f) {
    assert(f != NULL);
    DEBUG({
        tab_prefix(_level) << "Evaluating for Formula: "
                << f->toString() << std::endl;
    });
    const ConstantSymbol* x = f->x();
    const ConstantSymbol* y = f->y();
    assert(x != NULL && y != NULL);

    const PointMove *xmove = _alpha->get()->get(x);
    const PointMove *ymove = _alpha->get()->get(y);

    if (xmove != NULL && xmove == ymove) {
    	_game = determined.get(MCGame::VERIFIER);
	return;
    }
    if ((xmove == NULL && ymove != NULL) || (xmove != NULL && ymove == NULL)) {
    	_game = determined.get(MCGame::FALSIFIER);
	return;
    }
    AtomarUndetGame* returngame = new AtomarUndetGame(f);
    DEBUG(returngame->level(_level));
    _game = MCGameFlyFactory::make(returngame);
}

void AtomarGameFactory::visit(const NegatedFormula * f) {
    assert(f != NULL);
    DEBUG({
        tab_prefix(_level) << "Evaluating for Formula: "
                << f->toString() << std::endl;
    });
    AtomarGameFactory factory(_alpha);
    DEBUG(factory.level(_level+1));
    f->subformula()->accept(&factory);
    const MCGame_f* sub = factory.get();

    if (sub->get()->outcome() == MCGame::FALSIFIER) {
	_game = determined.get(MCGame::opponent<MCGame::FALSIFIER>::value);
    } else if (sub->get()->outcome() == MCGame::VERIFIER) {
	_game = determined.get(MCGame::opponent<MCGame::VERIFIER>::value);
    } if (sub->get()->outcome() == MCGame::UNDETERMINED) {
        // ceate a new game with the correct formula set.
        AtomarUndetGame* returngame = new AtomarUndetGame(f);
        DEBUG(returngame->level(_level));
        _game = MCGameFlyFactory::make(returngame);
    }
    delete sub;
}

} // namespace
