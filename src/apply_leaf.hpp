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
#include "apply_leaf.h"
#include "leaf_game_factory.h"

#include <algorithm>
#include <boost/scoped_ptr.hpp>

namespace sequoia {

template <typename Solver>
void ApplyLeaf<Solver>::init() {
    const TreeDecomposition* tdc = _solver->treedecomposition();
    _node_bag = tdc->bag(_node);
}

template <typename Solver>
void ApplyLeaf<Solver>::operator()() {

    // set the terminal symbols to null
    boost::scoped_ptr<const Assignment_f> alpha(_solver->base_alpha()->clone());
    for (unsigned int i = 0; i < _solver->n_terminals(); i++) {
        const ConstantSymbol *sym = _solver->terminal_symbol(i);
        alpha.reset(AssignmentFlyFactory::make(new ObjAssignment(alpha.get(), sym, NULL)));
    }
    _solver->alpha(_node, alpha.get());

    const std::vector<const UnarySymbol*> &free_vars = _solver->free_unary_symbols();
    for (unsigned int i = 0; i < free_vars.size(); ++i) {
	const UnarySymbol* sym = _solver->free_unary_symbol(i);
	const SetMove* smove = moves_pool.pool(new SetMove(sym));
	DPRINTLN("Assign '" << sym->identifier() << "' to: " << smove->toString());
        alpha.reset(AssignmentFlyFactory::make(new SetAssignment(alpha.get(), sym, smove)));
    }
    _solver->alpha(_node, alpha.get());

    // now construct the leaf game
    LeafGameFactory factory(alpha.get());
    DEBUG(factory.level(1));
    _solver->formula()->accept(&factory);
    const MCGame_f *res = factory.get();
    DPRINTLN("Result:");
    DEBUG(res->get()->recursive_print_game(0, true));
    DPRINTLN("");

    // store result in table
    const void *value = _solver->evaluation()->elem(_node_bag, alpha->get());
    _solver->table(_node)->update_value(alpha.get(), res, value);
}

} // namespace