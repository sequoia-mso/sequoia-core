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
#ifndef SEQUOIA_GAME_H
#define SEQUOIA_GAME_H

#include "flyweight.h"
#include "flyweight_inheritance.h"
#include "logic/assignment.h"
#include "logic/moves.h"

namespace sequoia {

class MCGame;
typedef FlyweightInheritance<MCGame> MCGame_f;
typedef FlyweightInheritanceFactory<MCGame> MCGameFlyFactory;

class MCGame {
public:
    enum Player { FALSIFIER = 0, VERIFIER, UNDETERMINED }; // order is important
    template <MCGame::Player> struct opponent {
	static MCGame::Player value;
    };

#define GAME_TYPE(x) ((x) == FALSIFIER ? "Univ" : "Exist")
    MCGame() : _formula(NULL), _hash(0UL) { }
    MCGame(const Formula *formula) : _formula(formula), _hash(0UL) {
        DEBUG(_level = 0);
    }
    virtual ~MCGame() { }
    virtual std::string toString() const {
        std::stringstream str;
        str << "[";
#ifdef DODEBUG
        str << name();
#else
        str << typeid(*this).name();
#endif
        str << "@" << this << ":"
                << "hash=" << hash()
                << ", formula=" << TOSTRING(_formula)
                << "]";
        return str.str();
    }
    size_t hash() const {
        if (_hash == 0UL)
	(const_cast<MCGame *>(this))->compute_hash();
	return _hash;
    }
    DEBUG(void level(int level) { this->_level = level; });
    DEBUG(int level() const { return _level; });
    /**
     * Recursively print a game.
     * @param level Indent to many tabs before each new line.
     * @param skip_first Do not indent the very first output.
     */
    virtual const MCGame_f* introduce(const ConstantSymbol* tsym,
    			              int signature_depth,
                                      const Assignment_f* alpha) const = 0;
    virtual const MCGame_f* forget(const ConstantSymbol* tsym,
    			           int signature_depth,
                                   const PointMove* replacement,
                                   const Assignment_f *alpha) const = 0;
    virtual const MCGame_f* join(const MCGame_f* other,
                                 const Assignment_f *alpha) const = 0;
    virtual const MCGame_f* convert() const = 0;
    DEBUG(virtual void recursive_print_game(int level, bool skip_first) const = 0);
    virtual Player outcome() const = 0;
    const Formula* formula() const { return _formula; }
protected:
    void hash(size_t hash) { _hash = hash; }
    virtual void compute_hash() = 0;
    DEBUG(virtual std::string name() const = 0);
private:
    const Formula *_formula;
    size_t _hash;
    DEBUG(const char* _name);
    DEBUG(int _level);
};
inline size_t hash_value(const MCGame& e) { return e.hash(); }
inline size_t hash_value(const MCGame_f& e) { return e.get()->hash(); }

}; // namespace

#endif // SEQUOIA_GAME_H
