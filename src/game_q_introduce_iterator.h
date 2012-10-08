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
#ifndef SEQUOIA_GAME_Q_INTRODUCE_ITERATOR_H
#define SEQUOIA_GAME_Q_INTRODUCE_ITERATOR_H

#include "game_map.h"

namespace sequoia {

template <class Derived, class TMove>
class QUndetGameIntroduceIteratorBase {
public:
    typedef GameMap<const TMove*, ptr_deep_hasher<TMove> > _MoveGameMap;
    typedef typename std::pair<const TMove*, const MCGame_f*> value_type;
    typedef const typename TMove::symbol_type* variable_type;
    typedef typename _MoveGameMap::const_iterator games_iterator;
    QUndetGameIntroduceIteratorBase<Derived, TMove>(
	const ConstantSymbol* tsym,
	int signature_depth,
	const Assignment_f* alpha,
	variable_type variable,
	const games_iterator &games,
	const games_iterator &gamesend)
    : _game_iterator(games, gamesend),
      _tsym(tsym),
      _signature_depth(signature_depth),
      _alpha(alpha),
      _variable(variable),
      _has_next(_game_iterator.has_next()) {
	DEBUG(_level = 0);
    }
    bool has_next() const { return _has_next; }
    value_type next() {
	return static_cast<Derived*>(this)->next();
    }
    DEBUG(void level(int level) { this->_level = level; });
    DEBUG(int level() const { return _level; });
protected:
    void has_next(bool has_next) { _has_next = has_next; }
    typedef typename _MoveGameMap::GameIterator GameIterator;
    GameIterator& game_iterator() { return _game_iterator; }
    const Assignment_f* alpha() { return _alpha; }
    int signature_depth() const { return _signature_depth; }
    const ConstantSymbol* tsym() const { return _tsym; }
    variable_type variable() const { return _variable; }
private:
    GameIterator _game_iterator;
    const ConstantSymbol* _tsym;
    int _signature_depth;
    const Assignment_f* _alpha;
    variable_type _variable;
    bool _has_next;
    DEBUG(int _level);
};
template <class TMove>
class QUndetGameIntroduceIterator : public QUndetGameIntroduceIteratorBase<
    QUndetGameIntroduceIterator<TMove>, TMove> {
public:
    typedef QUndetGameIntroduceIteratorBase<QUndetGameIntroduceIterator<TMove>, TMove> Base;
    typedef GameMap<const TMove*, ptr_deep_hasher<TMove> > _MoveGameMap;
    typedef typename _MoveGameMap::const_iterator games_iterator;
    typedef typename std::pair<const TMove*, const MCGame_f*> value_type;
    QUndetGameIntroduceIterator<TMove>(
	const ConstantSymbol* tsym,
	int signature_depth,
	const Assignment_f* alpha,
	const Symbol* variable,
	const games_iterator &games,
	const games_iterator &gamesend)
    : Base(tsym, signature_depth, alpha, variable, games, gamesend) { }
};
template <>
class QUndetGameIntroduceIterator<SetMove> : public QUndetGameIntroduceIteratorBase<
    QUndetGameIntroduceIterator<SetMove>, SetMove> {
public:
    QUndetGameIntroduceIterator<SetMove>(
	const ConstantSymbol* tsym,
	int signature_depth,
	const Assignment_f* alpha,
        variable_type variable,
	const games_iterator &games,
	const games_iterator &gamesend)
    : QUndetGameIntroduceIteratorBase<QUndetGameIntroduceIterator<SetMove>, SetMove>(
	tsym, signature_depth, alpha, variable, games, gamesend),
      _setbit(false), _cur_move(NULL), _cur_game(NULL) { }
public:
    value_type next();
private:
    bool _setbit;
    const SetMove* _cur_move;
    const MCGame_f* _cur_game;
};
template <>
class QUndetGameIntroduceIterator<PointMove> : public QUndetGameIntroduceIteratorBase<
    QUndetGameIntroduceIterator<PointMove>, PointMove> {
public:
    QUndetGameIntroduceIterator<PointMove>(
	const ConstantSymbol* tsym,
	int signature_depth,
	const Assignment_f* alpha,
        variable_type variable,
	const games_iterator &games,
	const games_iterator &gamesend)
    : QUndetGameIntroduceIteratorBase<QUndetGameIntroduceIterator<PointMove>, PointMove>(
	tsym, signature_depth, alpha, variable, games, gamesend),
      _repeat_last_move(false),
      _last_move(NULL),
      _last_game(NULL) { }
public:
    std::pair<const PointMove*, const MCGame_f*> next();
private:
    bool _repeat_last_move;
    const PointMove* _last_move;
    const MCGame_f* _last_game;
};

}; // namespace;

#endif // SEQUOIA_GAME_Q_INTRODUCE_ITERATOR_H
