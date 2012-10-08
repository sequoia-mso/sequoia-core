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

#include <assert.h>

namespace sequoia {
    
const MCGame_f* AtomarUndetGame::convert() const {
    std::cerr << "Error, convert() not defined for AtomarUndetGames!" << std::endl;
    assert(false);
    return NULL;
}

/*
 * Offload these to AtomarGameFactory to evaluate the atomic formula
 * based on the given alpha.
 * Caching is probably not worthwhile here, since the atomar evaluation
 * itself is cheap.
 */
const MCGame_f* AtomarUndetGame::forget(const ConstantSymbol* tsym,
					int signature_depth,
                                        const PointMove* replacement,
                                        const Assignment_f* alpha) const {
    AtomarGameFactory factory(alpha);
    DEBUG(factory.level(level()));
    formula()->accept(&factory);
    return factory.get();
}

const MCGame_f* AtomarUndetGame::introduce(const ConstantSymbol* tsym,
					   int signature_depth,
                                           const Assignment_f* alpha) const {
    AtomarGameFactory factory(alpha);
    DEBUG(factory.level(level()));
    formula()->accept(&factory);
    return factory.get();
}

const MCGame_f* AtomarUndetGame::join(const MCGame_f* other,
                                      const Assignment_f* alpha) const {
    AtomarGameFactory factory(alpha);
    DEBUG(factory.level(level()));
    formula()->accept(&factory);
    return factory.get();
}

} // namespace
