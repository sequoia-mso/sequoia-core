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
#include "../src/structures/labeled_graph.h"
#include "../src/structures/treedecomposition.h"
#include "../src/structures/graph_factory.h"
#include "../src/structures/graph_printer.h"
#include "../src/incidence_graph.h"

#include <iostream>
#include <fstream>

using sequoia::LabeledGraph;
using sequoia::TreeDecomposition;

int main(int argc, char **argv) {

    if (argc < 2 || argc == 4 || argc > 5) {
        std::cerr << "Usage:\t" << argv[0]
		<< " in_graph [out_graph] [in_treedecomp out_treedecomp]" << std::endl;
        exit(EXIT_FAILURE);
    }
    const char *ingraph = argv[1];
    const char *outgraph = NULL;
    if (argc == 3 || argc == 5)
        outgraph = argv[2];
    const char *intdc = NULL;
    const char *outtdc = NULL;
    if (argc == 5) {
        intdc = argv[3];
        outtdc = argv[4];
    }

    // load graph
    LabeledGraph g;
    try {
	sequoia::GraphFactory<LabeledGraph>::load_graph(g, ingraph);
    } catch (const std::exception &e) {
	std::cerr << "Error loading graph file: " << e.what() << std::endl;
	std::exit(EXIT_FAILURE);
    }
    TreeDecomposition tdc;
    if (intdc != NULL) {
	try {
	    sequoia::GraphFactory<TreeDecomposition>::load_graph(tdc, intdc);
	} catch (const std::exception &e) {
	    std::cerr << "Error loading tree decomposition file: " << e.what() << std::endl;
	    std::exit(EXIT_FAILURE);
	}
    }

    sequoia::LabeledGraph outg;
    if (intdc != NULL) 
        create_incidence_graph(g, outg, &tdc);
    else
        create_incidence_graph(g, outg, NULL);

    typedef sequoia::GraphPrinter<LabeledGraph> GP;
    typedef sequoia::GraphPrinter<TreeDecomposition> TP;

    try {
	if (outgraph != NULL)
	    GP::write_graph(outgraph, outg);
	else
	    GP::write_graph(std::cout, sequoia::GraphFileFormats::GRAPHML, outg);
    } catch (const std::exception &e) {
	std::cout << "Error writing graph file: " << e.what() << std::endl;
	std::exit(EXIT_FAILURE);
    }
    try {
	if  (outtdc != NULL)
	    TP::write_graph(outtdc, tdc);
    } catch (const std::exception &e) {
	std::cout << "Error writing tree decomposition file: " << e.what() << std::endl;
	std::exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
