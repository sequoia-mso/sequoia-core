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
#include "temp_symbol_factory.h"
#include "../config.h"
#include "unordered_defs.h"

namespace sequoia {

namespace internal {

struct temporary_symbol_factory_t {
    typedef SEQUOIA_CONCUR_UNORDERED_MAP<
        int,
	const ConstantSymbol*
    > Container;
    temporary_symbol_factory_t() { }
    const ConstantSymbol *get(unsigned int depth) {
	ConstantSymbol *entry = new ConstantSymbol("___temporary___", depth, true);
	std::pair<int, const ConstantSymbol*> e(depth, entry);
	std::pair<Container::const_iterator, bool> res = _container.insert(e);
	if (!res.second)
	    delete entry;
	return res.first->second;
    }
    Container _container;
} temporary_symbol_factory;

}

const ConstantSymbol *create_temporary_symbol(unsigned int depth) {
    return internal::temporary_symbol_factory.get(depth);
}

} // namespace
