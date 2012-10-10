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
#include "structures/graph.h"
#include "structures/graph_factory.h"
#include "structures/graph_printer.h"

#include <iostream>
#include <fstream>

int main(int argc, char **argv) {

    unsigned int size;
    const char *gfilename = NULL;
    float edgeprob = 1.0;
    unsigned int seed = 1;

    if (argc < 4) {
        std::cerr << "Usage: " << argv[0]
                << " graphfile size edgeprob [seed]"
                << std::endl;
        return 1;
    }
    gfilename = argv[1];
    size = atoi(argv[2]);

    edgeprob = atof(argv[3]);
    if (argc > 4)
        seed = atoi(argv[4]);

    typedef sequoia::Graph<> Graph;
    Graph g(size);
    sequoia::GraphFactory<Graph>::create_random_graph(g, size, edgeprob, seed);
    try {
	sequoia::GraphPrinter<Graph>::write_graph(gfilename, g);
    } catch (const std::exception &e) {
	std::cerr << "Error writing file: " << e.what() << std::endl;
	std::exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
