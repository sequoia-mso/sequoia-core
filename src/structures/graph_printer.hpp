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
#ifndef _GRAPH_PRINTER_HPP
#define	_GRAPH_PRINTER_HPP

#include "graph.h"
#include "graph_exceptions.h"
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

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

namespace sequoia {

template <typename Graph>
void GraphPrinter<Graph>::write_graph(const char *filename, const Graph &g) {
    boost::filesystem::path p(filename);
    std::string ext = boost::filesystem::extension(p);
    if (ext.empty())
	throw graph_unknown_file_format(std::string(filename));

    GraphFileFormats::Format format = GraphFileFormats::guess(ext);
    if (format == GraphFileFormats::UNKNOWN)
	throw graph_unknown_file_format(ext);

    write_graph(filename, format, g);
}

template <typename Graph>
void GraphPrinter<Graph>::write_graph(const char *filename,
				      GraphFileFormats::Format format,
				      const Graph &g) {
    std::fstream out(filename, std::fstream::out);
    if (!out.good())
	throw graph_file_error(std::string(filename));
    write_graph(out, format, g);
    out.close();
}

/**
 * Only GRAPHML and LEDA supported at the moment
 * @param outs
 * @param format
 * @param g
 */
template <typename Graph>
void GraphPrinter<Graph>::write_graph(std::ostream &outs,
				      GraphFileFormats::Format format,
				      const Graph &g) {
    typedef typename Graph::VertexPropertyConverter VertexConv;
    typedef typename Graph::EdgePropertyConverter EdgeConv;
    VertexConv vertex_conv;
    EdgeConv edge_conv;
    
    if (format == GraphFileFormats::LEDA) {
	write_graph_leda(outs, g);
	return;
    }
#if 0
    case DIMACS:
	write_graph_dimacs(outs, g);
	return;
#endif
    
    // setup dynamic properties object
    typename boost::dynamic_properties properties(boost::ignore_other_properties);

    // vertex properties
    typedef typename std::map<typename Graph::vertex_descriptor, std::string> Vertex2ValueMap;
    Vertex2ValueMap vertex2value;
    boost::associative_property_map<Vertex2ValueMap> vertex_propmap(vertex2value);
    if (vertex_conv.key_name() != NULL)
	properties.property(vertex_conv.key_name(), vertex_propmap);

    // edge properties
    typedef typename std::map<typename Graph::edge_descriptor, std::string> Edge2ValueMap;
    Edge2ValueMap edge2value;
    boost::associative_property_map<Edge2ValueMap> edge_propmap(edge2value);
    if (edge_conv.key_name() != NULL)
	properties.property(edge_conv.key_name(), edge_propmap);




    // fill with values from the graph
    typename Graph::vertex_iterator v, vend;
    for (boost::tie(v, vend) = g.vertices(); v != vend; v++) {
	if (vertex_conv.key_name() != NULL)
	    vertex_propmap[*v] = vertex_conv.get(g, *v);
	out_edge_iterator e, eend;
	for (boost::tie(e, eend) = g.out_edges(*v); e != eend; e++)
	    if (edge_conv.key_name() != NULL)
		edge_propmap[*e] = edge_conv.get(g, *e);
    } 

    // write out the graph
    if (format == GraphFileFormats::GRAPHML) {
	write_graph_graphml(outs, g, properties);
	return;
    }
#if HAVE_GRAPHVIZ
    if (format == GraphFileFormats::GRAPHVIZ) {
	write_graph_graphviz(outs, g, properties);
	return;
    }
#endif
}

template <typename Graph>
void GraphPrinter<Graph>::write_graph_leda(std::ostream &outs,
						 const Graph &g) {
    typedef typename Graph::VertexPropertyConverter VertexConv;
    typedef typename Graph::EdgePropertyConverter EdgeConv;
    VertexConv vertex_conv;
    EdgeConv edge_conv;

    outs << "LEDA.GRAPH" << std::endl;
    outs << "string" << std::endl; // Node type
    outs << "string" << std::endl; // Edge type
    outs << "-1" << std::endl;
    outs << g.num_vertices() << std::endl;
    for (unsigned int v = 0; v < g.num_vertices(); ++v)
	outs << "|{" << vertex_conv.get(g, v) << "}|" << std::endl;
    outs << g.num_edges() << std::endl;
    vertex_iterator v, vend;
    for (boost::tie(v, vend) = g.vertices(); v != vend; ++v) {
	out_edge_iterator n, nend;
	for (boost::tie(n, nend) = g.out_edges(*v); n != nend; ++n) {
	    edge_t e = *n;
	    outs << g.source(e)+1 << " " <<
		    g.target(e)+1 << " 0 |{" << 
		    edge_conv.get(g, e) << "}|" << std::endl;
	}
    }
}

} // namespace

#endif	/* _GRAPH_PRINTER_HPP */