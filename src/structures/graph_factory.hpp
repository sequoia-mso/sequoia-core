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
#ifndef _GRAPH_FACTORY_HPP
#define	_GRAPH_FACTORY_HPP

#include "graph_factory.h"
#include "graph_exceptions.h"

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

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

namespace sequoia {

template <typename Graph>
void GraphFactory<Graph>::create_random_graph(Graph &g, int size,
					      float prob, int seed) {
    typedef boost::erdos_renyi_iterator<boost::minstd_rand, typename Graph::GraphImpl> ERGen;
    boost::minstd_rand gen;
    gen.seed(seed);
    typedef typename Graph::GraphImpl GraphImpl;
    g.impl() = GraphImpl(ERGen(gen, size, prob), ERGen(), size);
}

template <typename Graph>
void GraphFactory<Graph>::load_graph(Graph &g, const char *filename) {
    boost::filesystem::path p(filename);
    std::string ext = boost::filesystem::extension(p);
    if (ext.empty())
	throw graph_unknown_file_format(std::string(filename));

    GraphFileFormats::Format format = GraphFileFormats::guess(ext);
    if (format == GraphFileFormats::UNKNOWN)
	throw graph_unknown_file_format(ext);

    load_graph(g, filename, format);
}

template <typename Graph>
void GraphFactory<Graph>::load_graph(Graph &g, const char *filename,
				     GraphFileFormats::Format format) {
    if (format == GraphFileFormats::UNKNOWN)
	// strange, why would we use this function??
	load_graph(g, filename);

    typedef typename Graph::VertexPropertyConverter VertexConv;
    typedef typename Graph::EdgePropertyConverter EdgeConv;
    VertexConv vertex_conv;
    EdgeConv edge_conv;

    // setup boost's dynamic properties
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
    load_graph(g, filename, format, properties);

    // write to property converter
    typename Graph::vertex_iterator v, vend;
    for (boost::tie(v, vend) = g.vertices(); v != vend; v++) {
	if (vertex_conv.key_name() != NULL)
	    vertex_conv.set(g, *v, vertex_propmap[*v]);
	out_edge_iterator e, eend;
	for (boost::tie(e, eend) = g.out_edges(*v); e != eend; e++)
	    if (edge_conv.key_name() != NULL)
		edge_conv.set(g, *e, edge_propmap[*e]);
    }
}

template <typename Graph>
void GraphFactory<Graph>::load_graph(Graph &g, const char *filename,
				     GraphFileFormats::Format format,
				     boost::dynamic_properties& properties) {
    boost::filesystem::path p(filename);
    if (!boost::filesystem::exists(p))
	throw graph_file_error(filename, "No such file or directory.");

    std::fstream in(filename, std::fstream::in);
    if (!in.good())
	throw graph_file_error(std::string(filename));

    switch (format) {
    case GraphFileFormats::GRAPHML:
	load_graphml(g, in, properties); 
	return;
    case GraphFileFormats::LEDA:
	load_leda(g, in);
	return;
    case GraphFileFormats::DIMACS:
	load_dimacs(g, in);
	return;
#if HAVE_GRAPHVIZ
    case GraphFileFormats::GRAPHVIZ:
	load_graphviz(g, in, properties);
	return;
#endif
    default: 
	assert(false);
    }
}

} // namespace

#endif	/* _GRAPH_FACTORY_HPP */
