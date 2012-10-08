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
#ifndef _GRAPH_PRINTER_H
#define	_GRAPH_PRINTER_H

#include "graph.h"
#include "graph_file_formats.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_list_io.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/version.hpp>

namespace sequoia {

template <typename Graph>
class GraphPrinter {
private:
    typedef typename Graph::vertex_descriptor vertex_t;
    typedef typename Graph::edge_descriptor edge_t;
    typedef typename Graph::vertex_iterator vertex_iterator;
    typedef typename Graph::out_edge_iterator out_edge_iterator;
public:
    static void
    write_graph(const char *filename, const Graph &g);

    static void
    write_graph(const char *filename, GraphFileFormats::Format format,
		const Graph &g);

    static void
    write_graph(std::ostream &outs, GraphFileFormats::Format format,
		const Graph &g);
private:
    static void
    write_graph_graphml(std::ostream &outs, const Graph &g,
		        boost::dynamic_properties &properties) {
	boost::write_graphml(outs, g.impl(), properties, true);
    }
#if HAVE_GRAPHVIZ
    static void
    write_graph_graphviz(std::ostream &outs, const Graph &g,
		        boost::dynamic_properties &properties) {
	properties.property("node_id", boost::get(boost::vertex_index_t(), g.impl()));
	boost::write_graphviz_dp(outs, g.impl(), properties);
    }
#endif

    static void
    write_graph_leda(std::ostream &outs, const Graph &g);
};

} // namespace

#include "graph_printer.hpp"

#endif	/* _GRAPH_PRINTER_H */