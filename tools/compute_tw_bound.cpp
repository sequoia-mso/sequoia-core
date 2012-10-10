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
#include "mindegree_heuristic.h"
#include "structures/treedecomposition.h"
#include "structures/labeled_graph.h"
#include "structures/graph_factory.h"
#include "structures/treedecomposition_check.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>

using namespace sequoia;

void usage(char *name) {
    std::cerr << "Usage: " << name << " <graph file>" << std::endl;
    std::exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
    if(argc < 2) {
        std::cerr << "Error:  No input file given" << std::endl;
        usage(argv[0]);
    }

    LabeledGraph g;
    try {
	GraphFactory<LabeledGraph>::load_graph(g, argv[1]);
    } catch (const std::exception &e) {
	std::cerr << "Error loading graph: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    MinDegreeHeuristic<LabeledGraph> a(g);
    a.compute();
    TreeDecomposition *tdc = a.get();
    if (tdc == NULL) {
        std::cerr << "Error converting file " << argv[1] << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << tdc->width() - 1 << std::endl;
    return EXIT_SUCCESS;
}
