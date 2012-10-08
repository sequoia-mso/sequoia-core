//=======================================================================
// Copyright 2007 Aaron Windsor
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#ifndef SEQUOIA_STRUCTURES_GRAPH_FACTORY_DIMACS_HPP
#define SEQUOIA_STRUCTURES_GRAPH_FACTORY_DIMACS_HPP

#include "graph.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <utility>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>

namespace sequoia {

template <typename Graph>
void GraphFactory<Graph>::load_dimacs(Graph &g, std::istream& in) {
    std::vector<vertex_t> vertices_by_index;
    std::map<size_t, vertex_t> nodemap;
    int nodelines = 0;
    while (!in.eof()) {
        char buffer[256];
        in.getline(buffer, 256);
        std::string s(buffer);
        if (s.size() == 0) continue;
        std::vector<std::string> v;
        boost::split(v, buffer, boost::is_any_of(" \t\n\r"));
		//std::cerr << "line is: " << buffer << std::endl;
        if (v[0] == "p") {
            //v[1] == "edge"
	    size_t num_v = boost::lexical_cast<std::size_t > (v[2].c_str());
            g = Graph(num_v);
            std::copy(g.vertices().first,
                      g.vertices().second,
                      std::back_inserter(vertices_by_index));
	    for(size_t i = 0; i < num_v; i++)
               nodemap[i+1] = vertices_by_index[i];
        } else if (v[0] == "n") {
            std::size_t id = boost::lexical_cast<std::size_t > (v[1].c_str());
	    nodemap[id] = vertices_by_index[nodelines++];
        } else if (v[0] == "e") {
            std::size_t v1 = boost::lexical_cast<std::size_t > (v[1].c_str());
            std::size_t v2 = boost::lexical_cast<std::size_t > (v[2].c_str());
            typename std::map<size_t, vertex_t>::const_iterator it = nodemap.find(v1);
	    if (it == nodemap.end()) {
                std::cerr << "Invalid node id '" << v1 << "' in line " << buffer << std::endl;
		throw;
            }
	    vertex_t vert1 = it->second;
            it = nodemap.find(v2);
	    if (it == nodemap.end()) {
                std::cerr << "Invalid node id '" << v2 << "' in line " << buffer << std::endl;
		throw;
            }
	    vertex_t vert2 = it->second;
	    if (vert1 == vert2) {
                std::cerr << "Error, graphs with self-loops are not supported in line " << buffer << std::endl;
		throw;
            }
            g.add_edge(vert1, vert2);
        }
    }
}

} // namespace

#endif // SEQUOIA_STRUCTURES_GRAPH_FACTORY_DIMACS_HPP
