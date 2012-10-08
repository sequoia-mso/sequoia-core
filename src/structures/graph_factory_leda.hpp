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
#ifndef _GRAPH_FACTORY_LEDA_HPP
#define	_GRAPH_FACTORY_LEDA_HPP

#include "graph_factory.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <utility>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_list_io.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/version.hpp>

namespace sequoia {

namespace internal {
static std::istream& mygetline(std::istream& is, std::string& str) {
    std::istream &res = std::getline(is, str);
    if (res.fail())
	throw graph_syntax_error(std::string("Premature end of file reached"));
    return res;
}
} // namespace internal

template <typename Graph>
void GraphFactory<Graph>::load_leda(Graph &g, std::istream& in) {
    typedef typename Graph::VertexPropertyConverter Conv;
    typedef typename Graph::EdgePropertyConverter EdgeConv;
    Conv conv;
    EdgeConv edgeconv;
    std::vector<vertex_t> vertices_by_index;
    int numberNodes = -1;
    int numberEdges = -1;
    std::string line;
    std::stringstream sstream;
    
    // expect LEDA.GRAPH
    internal::mygetline(in, line);
    if (line.compare("LEDA.GRAPH") != 0)
	throw graph_syntax_error(std::string("Unexpected line: ") + line);

    // expect two lines of node/edge types; ignore
    internal::mygetline(in, line);
    internal::mygetline(in, line);
    // expect "-1" ((bi-)directed graphs)
    // or     "-2" (undirected)
    internal::mygetline(in, line);
    if (line.compare("-1") != 0 && line.compare("-2") != 0)
	throw graph_syntax_error(std::string("Unexpected line: ") + line);
    // expect number of nodes
    internal::mygetline(in, line);
    sstream.str(line); sstream.clear();
    sstream >> numberNodes;

#if BOOST_VERSION == 104800 // strange error in boost 1.48
    for (int vrtn = 0; vrtn < numberNodes; vrtn++)
	g.add_vertex();
#else
    g = Graph(numberNodes);
#endif
    std::copy(g.vertices().first,
	      g.vertices().second,
	      std::back_inserter(vertices_by_index));
    
    // numberNodes lines with node values
    for (int i = 0; i < numberNodes; ++i) {
	internal::mygetline(in, line);
	vertex_t v = vertices_by_index[i];
	conv.set(g, v, line.substr(2, line.length() - 4));
    }
    // expect number of edges
    internal::mygetline(in, line);
    sstream.str(line);
    sstream.clear();
    sstream >> numberEdges;
    for (int i = 0; i < numberEdges; ++i) {
	internal::mygetline(in, line);
	sstream.str(line); sstream.clear();
	int target, source;
	std::string properties, dummy;
	sstream >> source >> target >> dummy >> properties;
	edge_t e = g.add_edge(vertices_by_index[source-1], vertices_by_index[target-1]);
	if (properties.length() >= 4)
	    edgeconv.set(g, e, properties.substr(2, properties.length() - 4));
    }
}

} // namespace

#endif	/* _GRAPH_FACTORY_LEDA_HPP */
