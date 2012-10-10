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
#include "../src/structures/graph.h"
#include "../src/structures/graph_printer.h"
#include "../src/structures/treedecomposition.h"

int main(int argc, char **argv) {

    typedef sequoia::Graph<> Graph;

    unsigned int width;
    unsigned int length;
    const char *gfilename = NULL;
    const char *tfilename = NULL;
    float edgeprob = 1.0;
    unsigned int seed = 1;

    if (argc < 5) {
        std::cerr << "Usage: " << argv[0]
                << " graphfile tdcfile width length [edgeprob [seed]]"
                << std::endl;
        return 1;
    }
    gfilename = argv[1];
    tfilename = argv[2];
    width = atoi(argv[3]);
    length = atoi(argv[4]);

    if (argc > 5)
        edgeprob = atof(argv[5]);
    if (argc > 6)
        seed = atoi(argv[6]);

    srand(seed);

    if(width > length) {
        unsigned int tmp = width;
        width = length;
        length = tmp;
    }
    unsigned int numberNodes = width * length;
    Graph grid(numberNodes);

    /* due to the rand() % 1000, the probability
     * is never 1.0, but might be 0.0.
     * We therefore swap the edgeprob and the test,
     * such that edgeprob is the prob of skipping an edge. */
    edgeprob = 1.0 - edgeprob;

    for (unsigned int i = 0; i < numberNodes; i++) {
        if (i < (length - 1) * width) {
            int rval = rand() % 1000;
            if(rval/1000.0 >= edgeprob)
                grid.add_edge(i, i + width);
        }
        if (i % width != width - 1) {
            int rval = rand() % 1000;
            if(rval/1000.0 >= edgeprob)
                grid.add_edge(i, i+1);
        }
    }

    sequoia::TreeDecomposition tdc;
    sequoia::TreeDecomposition::vertex_descriptor old = 0, current;

    // border case for empty grids and 1x1 and 1x2 grids
    if (width == 0) {
        current = tdc.add_vertex();
    } else if (width == 1 && length <= 2) {
        current = tdc.add_vertex();
	for (unsigned int i = 0; i < numberNodes; i++)
	    tdc.bag(current)->add(i);
    } else {
	for (unsigned int i = 0; i < (length * width) - width; i++) {
	    current = tdc.add_vertex();
	    for(unsigned int j = 0; j < width+1; j++)
		tdc.bag(current)->add(i+j);
	    if(i > 0)
		tdc.add_edge(current, old);
	    else
		tdc.root(current);
	    old = current;
	}
    }

    typedef sequoia::GraphPrinter<Graph> GraphPrinter;
    typedef sequoia::GraphPrinter<sequoia::TreeDecomposition> TdcPrinter;
    try {
	GraphPrinter::write_graph(gfilename, grid);
	TdcPrinter::write_graph(tfilename, tdc);
    } catch (const std::exception &e) {
	std::cerr << "Error: " << e.what() << std::endl;
	std::exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
