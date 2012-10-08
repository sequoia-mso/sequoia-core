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
#ifndef GAME_Q_FORGET_ITERATOR_H
#define GAME_Q_FORGET_ITERATOR_H

#include "game_map.h"

namespace sequoia {

template <class Derived, class TMove>
class QUndetGameForgetIteratorBase {
public:
    typedef GameMap<const TMove*, ptr_deep_hasher<TMove> > _MoveGameMap;
    typedef typename std::pair<const TMove*, const MCGame_f*> value_type;
    typedef const typename TMove::symbol_type* variable_type;
    typedef typename _MoveGameMap::const_iterator games_iterator;
    QUndetGameForgetIteratorBase<Derived, TMove>(
	const ConstantSymbol* tsym,
	int signature_depth,
	const PointMove* replacement,
	const Assignment_f* alpha,
	variable_type variable,
	const games_iterator &games,
	const games_iterator &gamesend)
    : _game_iterator(games, gamesend),
      _tsym(tsym),
      _signature_depth(signature_depth),
      _replacement(replacement),
      _alpha(alpha),
      _variable(variable),
      _has_next(_game_iterator.has_next()),
      _last_oldmove(NULL),
      _last_modified(NULL),
      _last_replacement(NULL) {
	DEBUG(_level = 0);
    }
    bool has_next() const { return _has_next; }
    value_type next();
    const PointMove* adjust_replacement(const TMove* oldmove) {
	return static_cast<Derived*>(this)->adjust_replacement(oldmove);
    }
    const TMove* adjust_move(const TMove* oldmove) {
	return static_cast<Derived*>(this)->adjust_move(oldmove);
    }
    DEBUG(void level(int level) { this->_level = level; });
    DEBUG(int level() const { return _level; });
protected:
    void has_next(bool has_next) { _has_next = has_next; }
    typedef typename _MoveGameMap::GameIterator GameIterator;
    GameIterator& game_iterator() { return _game_iterator; }
    int signature_depth() const { return _signature_depth; }
    const Assignment_f* alpha() const { return _alpha; }
    const PointMove* replacement() const { return _replacement; }
    const ConstantSymbol* tsym() const { return _tsym; }
    variable_type variable() const { return _variable; }
private:
    GameIterator _game_iterator;
    const ConstantSymbol* _tsym;
    int _signature_depth;
    const PointMove* _replacement;
    const Assignment_f* _alpha;
    variable_type _variable;
    bool _has_next;
    DEBUG(int _level);
protected:
    const TMove *_last_oldmove;
    const TMove *_last_modified;
    const PointMove *_last_replacement;
};
template <class TMove>
class QUndetGameForgetIterator : public QUndetGameForgetIteratorBase<
    QUndetGameForgetIterator<TMove>, TMove> {
public:
    typedef QUndetGameForgetIteratorBase<QUndetGameForgetIterator<TMove>, TMove> Base;
    typedef GameMap<const TMove*, ptr_deep_hasher<TMove> > _MoveGameMap;
    typedef typename std::pair<const TMove*, const MCGame_f*> value_type;
    typedef typename TMove::symbol_type variable_type;
    typedef typename _MoveGameMap::const_iterator games_iterator;
    QUndetGameForgetIterator<TMove>(
	const ConstantSymbol* tsym,
	int signature_depth,
	const PointMove* replacement,
	const Assignment_f* alpha,
	variable_type variable,
	const games_iterator &games,
	const games_iterator &gamesend)
    : Base(tsym, signature_depth, replacement, alpha, variable, games, gamesend) { }
};
template <>
class QUndetGameForgetIterator<SetMove> : public QUndetGameForgetIteratorBase<
    QUndetGameForgetIterator<SetMove>, SetMove> {
public:
    typedef QUndetGameForgetIteratorBase<QUndetGameForgetIterator<SetMove>, SetMove> Base;
    QUndetGameForgetIterator<SetMove>(
	const ConstantSymbol* tsym,
	int signature_depth,
	const PointMove* replacement,
	const Assignment_f* alpha,
	variable_type variable,
	const games_iterator &games,
	const games_iterator &gamesend)
    : Base(tsym, signature_depth, replacement, alpha, variable, games, gamesend) { }
    const PointMove* adjust_replacement(const SetMove* oldmove);
    const SetMove* adjust_move(const SetMove* oldmove);
};
template <>
class QUndetGameForgetIterator<PointMove> : public QUndetGameForgetIteratorBase<
    QUndetGameForgetIterator<PointMove>, PointMove> {
public:
    typedef QUndetGameForgetIteratorBase<QUndetGameForgetIterator<PointMove>, PointMove> Base;
    QUndetGameForgetIterator<PointMove>(
	const ConstantSymbol* tsym,
	int signature_depth,
	const PointMove* replacement,
	const Assignment_f* alpha,
	variable_type variable,
	const games_iterator &games,
	const games_iterator &gamesend)
    : Base(tsym, signature_depth, replacement, alpha, variable, games, gamesend) { }
    const PointMove* adjust_replacement(const PointMove* oldmove);
    const PointMove* adjust_move(const PointMove *oldmove);
};

}; // namespace;

#include "game_q_forget_iterator.hpp"

#endif // GAME_Q_FORGET_ITERATOR_H
