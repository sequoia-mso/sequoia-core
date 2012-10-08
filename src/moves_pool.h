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
#ifndef SEQUOIA_MOVES_POOL_H
#define SEQUOIA_MOVES_POOL_H

#include "pooling.h"
#include "logic/moves.h"

namespace sequoia {

class MovesPool {
public:
    MovesPool() : _pool_setmoves(0), _pool_pointmoves(0) {
        for (unsigned int i = 0; i < BITSET_SIZE; i++) {
            _pool_setmoves.push_back(new Pool<SetMove>());
            _pool_pointmoves.push_back(new Pool<PointMove>());
        }
    }
    virtual ~MovesPool() {
        for (unsigned int i = 0; i < BITSET_SIZE; i++) {
            delete _pool_setmoves[i];
            delete _pool_pointmoves[i];
        }
    }
    const PointMove* pool(const PointMove* pmove) {
        return _pool_pointmoves[pmove->nesting_depth()]->pooling(pmove);
    }
    const SetMove* pool(const SetMove* smove) {
        return _pool_setmoves[smove->nesting_depth()]->pooling(smove);
    }
    /**
     * Slow, for debugging purposes only.
     * @return the number of stored objects.
     */
    size_t size() const {
        size_t s = 0;
        for (unsigned int i = 0; i < BITSET_SIZE; i++) {
            s += _pool_setmoves[i]->size();
            s += _pool_pointmoves[i]->size();
        }
        return s;
    }
private:
    std::vector<Pool<SetMove> *> _pool_setmoves;
    std::vector<Pool<PointMove> *> _pool_pointmoves;
};

extern MovesPool moves_pool;

} // namespace

#endif // SEQUOIA_MOVES_POOL_H