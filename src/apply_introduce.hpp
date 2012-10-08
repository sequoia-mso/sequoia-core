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
#include "apply_introduce.h"

namespace sequoia {

template <typename Solver>
void ApplyIntroduce<Solver>::spec_init() {
    const LabeledGraph* graph = Base::_solver->graph()->graph();
    for (unsigned int i = 0; i < Base::_node_bag->width(); i++) {
        // XXX undirected only
        if (graph->is_out_neighbor(Base::_node_bag->get(Base::_terminal), Base::_node_bag->get(i)) ||
            graph->is_out_neighbor(Base::_node_bag->get(i), Base::_node_bag->get(Base::_terminal)))
            _neighbors.set(i, true);
    }
    LabeledGraph::vertex_descriptor vertex = Base::_node_bag->get(Base::_terminal);
    for (unsigned int i = 0; i < graph->num_labels(); i++)
        if (graph->vertex_has_label(vertex, i))
            _labels.set(i, true);
    DPRINTLN("find_neighbors_and_labels: neighbors="
	    << _neighbors << ", labels=" << _labels);
}

template <typename Solver>
const PointMove*
ApplyIntroduce<Solver>::create_tmove() const {
    PointMove* tmptmove = new PointMove(Base::_terminal_sym);
    // set neighbors
    for (unsigned int i = 0; i < Base::_terminal; ++i) {
        // XXX undirected only
	if (_neighbors.test(i))
	    tmptmove->add_edge(Base::_solver->terminal_symbol(i));
    }
    // set labels
    const GraphStructure* structure = Base::_solver->graph();
    const Vocabulary* gvoc = structure->vocabulary();
    for (unsigned int i = 0; i < gvoc->number_of_unary_symbols(); ++i) {
	const UnarySymbol* sym = gvoc->unary_symbol(i);
	assert(sym != NULL);
	if (structure->has_label(Base::_node_bag->get(Base::_terminal), sym))
	    tmptmove->add_label(sym);
    }
    return moves_pool.pool(tmptmove);
}

template <typename Solver>
const PointMove*
ApplyIntroduce<Solver>::update_pointmove_impl(unsigned int i) const {
    const Assignment_f *child_alpha = Base::_solver->alpha(Base::_child);
    
    if (i < Base::_terminal)
	// nothing changed, terminal i is still symbol t_i...
	return child_alpha->get()->get(Base::_solver->terminal_symbol(i));

    if (i == Base::_terminal)
	// already created above
	return create_tmove();

    // i > Base::_terminal: terminal i was terminal i-1 before, fix this
    const PointMove* oldmove = child_alpha->get()->get(Base::_solver->terminal_symbol(i-1));
    if (oldmove == NULL) return NULL;
    
    bool adjacent = _neighbors.test(i);
    PointMove* tmp = oldmove->rename_introduce(Base::_terminal_sym,
					       Base::_solver->terminal_symbol(i),
					       adjacent);
    return moves_pool.pool(tmp);
}

} // namespace