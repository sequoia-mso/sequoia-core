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
#ifndef GAME_ATOMAR_H
#define GAME_ATOMAR_H

#include "game.h"

namespace sequoia {

/**
 * AtomarUndetGame depends on the formula.
 * @param formula
 */
class AtomarUndetGame : public MCGame {
public:
    AtomarUndetGame() { }
    AtomarUndetGame(const Formula* formula)
        : MCGame(formula) {
    }
    bool operator==(const AtomarUndetGame &ug) const {
        if (this->formula() != ug.formula()) return false;
        return true;
    }
    virtual const MCGame_f* introduce(const ConstantSymbol* tsym,
    				      int signature_depth,
                                      const Assignment_f* alpha) const;
    virtual const MCGame_f* forget(const ConstantSymbol* tsym,
    				   int signature_depth,
                                   const PointMove* replacement,
                                   const Assignment_f* alpha) const;
    virtual const MCGame_f* join(const MCGame_f* other,
                                 const Assignment_f* alpha) const;
    virtual const MCGame_f* convert() const;
    DEBUG(virtual void recursive_print_game(int level, bool skip_first) const {
	(skip_first ? std::cout : tab_prefix(level)) << toString() << std::endl;
    });
    virtual Player outcome() const { return MCGame::UNDETERMINED; }
protected:
    virtual void compute_hash() {
        size_t h = hash_init();
	hash_combine(h, formula()->hash());
	hash(h);
    }
    DEBUG(virtual std::string name() const { 
        return std::string("AtomarUndetGame[") + formula()->toString() + "]";
    });
};

}; // namespace

#endif // GAME_ATOMAR_H
