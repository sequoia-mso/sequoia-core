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
#ifndef GAME_BOOL_COMB_H
#define GAME_BOOL_COMB_H

#include "game.h"
#include "game_set.h"
#include "unordered_defs.h"

namespace sequoia {

template <typename TFormula, MCGame::Player _player>
class BoolCombUndetGame : public MCGame {
    /**
     * This class contains the pointers to the flyweights of the subgames.
     * On destruction, the class frees the pointers to the flyweights.
     */
    class SubgamesContainer : public SEQUOIA_UNORDERED_MAP<
        const Formula*,
        const MCGame_f*,
        ptr_deep_hasher<Formula> > {
    public:
        SubgamesContainer() { }
        ~SubgamesContainer() {
            const_iterator it = begin();
            const_iterator itend = end();
            for (; it != itend; it++)
                delete it->second;
        }
        bool operator==(const SubgamesContainer& other) const {
            if (this->size() != other.size()) return false;
            const_iterator it = begin();
            const_iterator itend = end();
            const_iterator other_end = other.end();
            for (; it != itend; it++) {
                const_iterator oit = other.find(it->first);
                if (oit == other_end) return false;
                assert(it->second != NULL);
                assert(oit->second != NULL);
                if (*it->second != *oit->second) return false;
            }
            return true;
        }
    private:
        /* forbid these */
        SubgamesContainer(const SubgamesContainer& other) { };
        SubgamesContainer& operator=(const SubgamesContainer& other) { return *this; };
    };
public:
    BoolCombUndetGame<TFormula, _player>(const TFormula* formula)
    : MCGame(formula) { }
    typedef typename SubgamesContainer::const_iterator subgames_iterator;
    typedef std::pair<subgames_iterator, subgames_iterator> subgames_range_t;
    subgames_range_t subgames() const {
	return std::make_pair(_subgames.begin(), _subgames.end());
    }
    const MCGame_f* subgame(const Formula* subf) const {
	typename SubgamesContainer::const_iterator it = _subgames.find(subf);
	if (it == _subgames.end()) return NULL; else return it->second;
    }
    bool empty() const { return _subgames.size() == 0; }
    const MCGame_f* add_subgame(const Formula *f, const MCGame_f* game);
    bool operator==(const BoolCombUndetGame& ug) const {
        if (hash() != ug.hash()) return false;
        if (this->formula() != ug.formula()) return false;
        return _subgames == ug._subgames;
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
    DEBUG(virtual void recursive_print_game(int level, bool skip_first) const);
    const TFormula* formula() const { return (const TFormula*) MCGame::formula(); }
    const MCGame_f* minimize() const;
    virtual Player outcome() const { return MCGame::UNDETERMINED; }
protected:
    virtual void compute_hash() {
        size_t h = hash_init();
	hash_combine(h, formula()->hash());
	subgames_iterator it = _subgames.begin();
	subgames_iterator itend = _subgames.end();
	for (; it != itend; it++) {
	    hash_combine(h, it->first->hash());
	    hash_combine(h, it->second->get()->hash());
	}
	hash(h);
    }
    void set_subgame(const Formula *f, const MCGame_f* game) {
        assert(game != NULL);
        _subgames[f] = game;
    }
    DEBUG(virtual std::string name() const { return std::string(GAME_TYPE(_player)) + "BoolCombUndetGame"; });
private:
    SubgamesContainer _subgames;
};
typedef BoolCombUndetGame<ConjBoolCombFormula, MCGame::FALSIFIER> ConjBoolCombUndetGame;
typedef BoolCombUndetGame<DisjBoolCombFormula, MCGame::VERIFIER> DisjBoolCombUndetGame;

}; // namespace

#include "game_bool_comb.hpp"

#endif // GAME_BOOL_COMB_H
