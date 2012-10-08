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
#ifndef SEQUOIA_SEQUOIA_TABLE_H
#define SEQUOIA_SEQUOIA_TABLE_H

#include "evaluation.h"
#include "game.h"
#include "hashing.h"
#include "structures/treedecomposition.h"
#include "sequoia_eval.h"

#include <boost/iterator/iterator_facade.hpp>
#include <boost/tuple/tuple.hpp>

#ifdef HAVE_TBB
#include <tbb/atomic.h>
#endif

namespace sequoia {

/**
 * Map a game to the ring element under an evaluation;  The void *
 * is to be casted to arbitrary values by the evaluation we used.
 * Corresponds to $val(G)$ in the KLR11 paper, where G is the game.
 */
class GameVoidPtrMap {
#ifdef HAVE_TBB
    typedef tbb::atomic<const void*> Value;
#else
    typedef const void* Value;
#endif
    typedef SEQUOIA_CONCUR_UNORDERED_MAP<
	const MCGame_f*,
	Value,
        ptr_deep_hasher<MCGame_f, flyweight_inheritance_deep_hasher<MCGame> >,
	ptr_deep_equals<const MCGame_f*>
    > Container;
public:
    typedef Container::value_type value_type;
    typedef Container::const_iterator const_iterator;
    GameVoidPtrMap() { }
    ~GameVoidPtrMap() {
	const_iterator it = begin();
	const_iterator itend = end();
	for (; it != itend; it++)
	    delete it->first;
    }
    const_iterator begin() const { return _container.begin(); }
    const_iterator end() const { return _container.end(); }
    
    void update_value(const MCGame_f* game, const void *val,
		      SequoiaInternalEvaluation *eval) {
        assert(game != NULL);
        if (game->get()->outcome() == MCGame::FALSIFIER) {
            delete game;
            return;
        }
	Value save_value; // tbb atomic needs default init first
	save_value = val;
        GameVoidPtrMap::value_type entry(std::make_pair(game, save_value));
        std::pair<Container::iterator, bool> res = _container.insert(entry);
        if (!res.second) { // existing entry
	    DEBUG({
		std::cout << "duplicate entry for game: " << game->get()->toString() << std::endl;
		std::cout << "previous value: " << eval->toString(res.first->second) << std::endl;
		std::cout << "compare  value: " << eval->toString(val) << std::endl;
	    });
	    // clean up the game, no longer needed
            delete game;
	    // existing entry, use _evaluation to decide compute new value
#ifdef HAVE_TBB
	    const void *oldval, *newval;
	    do {
		oldval = res.first->second; // take snapshop
		newval = eval->add(oldval, val);
	    } while (res.first->second.compare_and_swap(newval, oldval) != oldval);
#else
	    res.first->second = eval->add(res.first->second, val);
#endif
        }
        DPRINTLN("update_value: new/old value: " << eval->toString(res.first->second));
    }
private:
    Container _container;
};

/**
 * This data structure is the table the algorithm builds for each
 * node during dynamic programming.  Essentially, this is a set of sets
 * that is indexed by an Assignment.
 * This corresponds to the sets $S(\bar U)$ in the KLR11 paper,
 * where $\bar U$ is the SetMoves array.
 */
class SequoiaTable {
private:
    typedef SEQUOIA_CONCUR_UNORDERED_MAP<
        const Assignment_f*,
        GameVoidPtrMap*,
        ptr_deep_hasher<
            Assignment_f,
            flyweight_inheritance_deep_hasher<Assignment>
        >,
        ptr_deep_equals<const Assignment_f*>
    > Container;
public:
    SequoiaTable(const TreeDecomposition* treedecomposition,
		 const TreeDecomposition::vertex_descriptor& node,
		 SequoiaInternalEvaluation *evaluation)
    : _treedecomposition(treedecomposition), _node(node),
      _evaluation(evaluation) { }
    ~SequoiaTable() {
	Container::const_iterator it;
	for(it = _container.begin(); it != _container.end(); it++) {
            delete it->first;
	    delete it->second;
        }
    }

    typedef Container::const_iterator const_iterator;
    typedef Container::key_type key_type;
    typedef Container::mapped_type mapped_type;
    typedef Container::value_type value_type;
    const_iterator begin() const { return _container.begin(); }
    const_iterator end() const { return _container.end(); }
    const_iterator find(const Assignment_f* alpha) const {
	return _container.find(alpha);
    }

    /**
     * Takes care of deleting the game, do not delete elsewhere!
     */
    void update_value(const Assignment_f* alpha,
		      const MCGame_f* game,
		      const void* val);
private:
    Container _container;
    const TreeDecomposition* _treedecomposition;
    const TreeDecomposition::vertex_descriptor _node;
    SequoiaInternalEvaluation *_evaluation;
};

} // namespace

#endif // SEQUOIA_SEQUOIA_TABLE_H
