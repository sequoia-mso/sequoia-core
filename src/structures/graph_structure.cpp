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
#include "graph_structure.h"

namespace sequoia {

GraphStructure::GraphStructure(const LabeledGraph* graph) {
    GraphVocabulary* voc = new GraphVocabulary();
    for (unsigned int i = 0; i < graph->num_labels(); ++i) {
        const std::string& name = graph->label(i);
        voc->add_unary_symbol(name, false);
    }
    Structure::vocabulary(voc);
    _graph = graph;
}

} // namespace
