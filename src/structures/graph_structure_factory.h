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
#ifndef SEQUOIA_GRAPH_STRUCTURE_FACTORY_H
#define SEQUOIA_GRAPH_STRUCTURE_FACTORY_H

#include "graph_structure.h"
#include "labeled_graph.h"

namespace sequoia {

class GraphStructureFactory {
public:
    static GraphStructure* load(const char *filename);
    static GraphStructure* make(const LabeledGraph *graph);
};

}

#endif // SEQUOIA_GRAPH_STRUCTURE_FACTORY_H