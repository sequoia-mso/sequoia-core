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
#ifndef SEQUOIA_GAME_DETERMINED_H
#define SEQUOIA_GAME_DETERMINED_H

#include "game.h"

namespace sequoia {

class DeterminedGame : public MCGame {
public:
    DeterminedGame(MCGame::Player outcome) : _outcome(outcome)  { }
    bool operator==(const MCGame &g) const {
        return outcome() == g.outcome();
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
    virtual Player outcome() const { return _outcome; }
    DEBUG(virtual void recursive_print_game(int level, bool skip_first) const);
    DEBUG(virtual std::string name() const { return ((outcome() == FALSIFIER) ? "FalseGame" : "TrueGame"); });
protected:
    virtual void compute_hash() {
        size_t h = hash_init();
	hash_combine(h, outcome());
	hash(h);
    }
private:
    MCGame::Player _outcome;
};

class DeterminedGameFactory {
public:
    const MCGame_f* get(MCGame::Player player) {
	const MCGame_f* g = games();
	return g[player].clone();
    }
    const MCGame_f* games() {
	static MCGame_f g[2] = {
	    // order is important, must match MCGame::Player
	    MCGameFlyFactory::make_static(new DeterminedGame(MCGame::FALSIFIER)),
	    MCGameFlyFactory::make_static(new DeterminedGame(MCGame::VERIFIER))
	};
	return g;
    }
#if 0 // this would require a bit of refactoring
    template <MCGame::Player _player>
    const MCGame_f* get() const  {
	static MCGame_f res(MCGameFlyFactory::make_static(new DeterminedGame(_player)));
	return res.clone();
    }
#endif
};

extern DeterminedGameFactory determined;

}; // namespace

#endif // SEQUOIA_GAME_DETERMINED_H
