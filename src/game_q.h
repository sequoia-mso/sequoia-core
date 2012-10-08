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
#ifndef SEQUOIA_GAME_Q_H
#define SEQUOIA_GAME_Q_H

#include "game.h"
#include "game_map.h"

#include <boost/iterator/iterator_facade.hpp>

namespace sequoia {

template <typename TFormula, typename TMove, MCGame::Player _player> 
class QUndetGame : public MCGame {
public:
    QUndetGame<TFormula, TMove, _player>(const TFormula* formula)
    : MCGame(formula) { }
    bool empty() const { return _subgames.size() == 0; }
    typedef GameMap<const TMove*, ptr_deep_hasher<TMove> > GamesContainer;
    typedef typename GamesContainer::const_iterator const_iterator;
    typedef typename GamesContainer::const_subgames_iterator subgames_iterator;
    const_iterator begin() const { return _subgames.begin(); }
    const_iterator end() const { return _subgames.end(); }
    const_iterator find(const TMove* move) const { return _subgames.find(move); }

    const MCGame_f* minimize() const;
    const MCGame_f* add_subgame(const TMove *move, const MCGame_f* game);
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

    bool operator==(const QUndetGame<TFormula, TMove, _player> &sg) const {
        if (hash() != sg.hash()) return false;
        if (this->formula() != sg.formula()) return false;
	return _subgames == sg._subgames;
    }
    const TFormula* formula() const { return (const TFormula*) MCGame::formula(); }
    DEBUG(virtual void recursive_print_game(int level, bool skip_first) const);
    virtual Player outcome() const { return MCGame::UNDETERMINED; }
protected:
    DEBUG(virtual std::string name() const { return std::string(GAME_TYPE(_player)) + typeid(TFormula).name() + "QUndetGame";});
    virtual void compute_hash() {
        size_t h = hash_init();
	hash_combine(h, formula()->hash());
	const_iterator it = begin();
	const_iterator itend = end();
	for (; it != itend; it++) {
	    size_t move_h = (it->first == NULL ? 1 : it->first->hash());
	    hash_combine(h, move_h);
	    hash_combine(h, it->second->hash());
	}
	hash(h);
    }
private:
    void set_subgame(const TMove *move, const MCGame_f* game);
    template<typename T> struct tag { };
    const MCGame_f* join_impl(const MCGame_f* other, const Assignment_f *alpha, tag<SetMove>) const;
    const MCGame_f* join_impl(const MCGame_f* other, const Assignment_f *alpha, tag<PointMove>) const;
    GamesContainer _subgames;
};

typedef QUndetGame<UnivSetQFormula, SetMove, MCGame::FALSIFIER> UnivSetQUndetGame;
typedef QUndetGame<ExistSetQFormula, SetMove, MCGame::VERIFIER> ExistSetQUndetGame;
typedef QUndetGame<UnivObjQFormula, PointMove, MCGame::FALSIFIER> UnivObjQUndetGame;
typedef QUndetGame<ExistObjQFormula, PointMove, MCGame::VERIFIER> ExistObjQUndetGame;

}; // namespace

#include "game_q.hpp"

#endif // SEQUOIA_GAME_Q_H
