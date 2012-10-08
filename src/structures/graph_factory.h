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
#ifndef _GRAPH_FACTORY_H
#define	_GRAPH_FACTORY_H

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
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/version.hpp>

namespace sequoia {

template <typename Graph>
class GraphFactory {
    typedef typename Graph::vertex_descriptor vertex_t;
    typedef typename Graph::edge_descriptor edge_t;
    typedef typename Graph::vertex_iterator vertex_iterator;
    typedef typename Graph::out_edge_iterator out_edge_iterator;
public:
    static void
    create_random_graph(Graph &g, int size, float prob, int seed);

    static void
    load_graph(Graph &g, const char *filename);

    static void
    load_graph(Graph &g, const char *filename, GraphFileFormats::Format format);

private:
    static void
    load_graph(Graph &g, const char *filename, GraphFileFormats::Format format,
	       boost::dynamic_properties& properties);
    static void
    load_graphml(Graph &g, std::istream& in, boost::dynamic_properties& properties);
#if HAVE_GRAPHVIZ
    static void
    load_graphviz(Graph &g, std::istream& in, boost::dynamic_properties& properties);
#endif

    /**
     * XXX The load_*_graph() functions are not very error prone, improve!
     * Read a graph in DIMACS from istream i and save into g.
     * @param g
     * @param in
     */
    static void
    load_dimacs(Graph &g, std::istream& in);

    /**
     * Read a graph in LEDA file format from istream i and save into g.
     * Beware:  We currently cannot handle comments "# ..." and/or empty lines
     * @param g
     * @param in
     */
    static void
    load_leda(Graph &g, std::istream& in);

};

} // namespace

#include "graph_factory.hpp"
#include "graph_factory_dimacs.hpp"
#include "graph_factory_leda.hpp"
#include "graph_factory_graphml.hpp"
#if HAVE_GRAPHVIZ
#include "graph_factory_graphviz.hpp"
#endif

#endif	/* _GRAPH_FACTORY_H */
