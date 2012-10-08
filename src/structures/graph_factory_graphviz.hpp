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
#ifndef SEQUOIA_STRUCTURES_GRAPH_FACTORY_GRAPHVIZ_HPP
#define SEQUOIA_STRUCTURES_GRAPH_FACTORY_GRAPHVIZ_HPP

#include "graph.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/copy.hpp>
#include <boost/shared_ptr.hpp>

namespace sequoia {

template <typename Graph>
void GraphFactory<Graph>::load_graphviz(Graph &g, std::istream& in,
				        boost::dynamic_properties& properties) {
    g.clear();
    try {
	bool undirected_input = false;
	try {
	    boost::read_graphviz(in, g.impl(), properties);
	    return;
	} catch (typename boost::undirected_graph_error &ex) {
	    undirected_input = true;
	}
	if (undirected_input) {
	    UGraph tmp;
	    in.seekg(0UL);
	    boost::read_graphviz(in, tmp, properties);
	    g.adapt(tmp);
	}
    } catch (boost::bad_graphviz_syntax e) {
	throw graph_syntax_error(std::string(e.what()));
    }
}

} // namespace

#endif // SEQUOIA_STRUCTURES_GRAPH_FACTORY_GRAPHVIZ_HPP