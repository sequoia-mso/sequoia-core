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
#include "structures/labeled_graph.h"
#include "structures/graph_factory.h"

#include <boost/graph/connected_components.hpp>

#include <iostream>
#include <fstream>

using namespace sequoia;
using namespace boost;

template <typename Graph, typename VertexIndexMap>
void test_connected(Graph &g, VertexIndexMap vm) {
    typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator_t;
    typedef typename graph_traits<Graph>::vertex_descriptor vertex_t;
    typedef typename graph_traits<Graph>::vertices_size_type v_size_t;
    typedef iterator_property_map< typename std::vector<v_size_t>::iterator,
                                   VertexIndexMap
                                  > vertex_to_v_size_map_t;

    std::vector<v_size_t> component_vector(num_vertices(g));
    vertex_to_v_size_map_t component(component_vector.begin(), vm);
    std::vector<vertex_t> vertices_by_component(num_vertices(g));

    v_size_t num_components = connected_components(g, component);

    std::cout << num_components << std::endl;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                << " infile [outfile]"
                << std::endl;
        return 1;
    }
    LabeledGraph g;
    try {
	GraphFactory<LabeledGraph>::load_graph(g, argv[1]);
    } catch (const std::exception &e) {
	std::cerr << "Error loading graph: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    test_connected(g.impl(), boost::get(boost::vertex_index,g.impl()));
    return 0;
}
