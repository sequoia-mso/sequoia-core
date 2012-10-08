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
#ifndef _GRAPH_H
#define	_GRAPH_H

// TODO remove unused includes
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
#include <boost/lexical_cast.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/version.hpp>

namespace sequoia {

/**
 * An adaptor of boost's BGL.  We essentially make some of boost's global
 * functions operating on graphs accessible as member functions without
 * giving up (too much) compatiblity to boost's algorithms when we need
 * them.  This class will be used internally in both, labeled graphs
 * (graph structures) and tree decompositions.
 * 
 * Note:  For sequoia internal use only, interface might change w/o prior notice!
 */

typedef boost::adjacency_list<
    boost::setS,	    // no parallel edges
    boost::vecS,	    // we use vertex indices everywhere
    boost::undirectedS	    // undirected graph
> UGraph;

struct NoProperty {
   friend std::ostream& operator<<(std::ostream& outs, const NoProperty& p) {
       return outs;
   }
   friend std::istream& operator>>(std::istream& ins, const NoProperty& p) {
       return ins;
   }
};

template <typename VertexProp = NoProperty, typename EdgeProp = NoProperty>
class Graph {
public:
    typedef boost::adjacency_list<
        boost::setS,		// no parallel edges
        boost::vecS,		// we use vertex indices everywhere
        boost::bidirectionalS,  // directed graph
	VertexProp,		// vertex properties
	EdgeProp		// edge properties
    > GraphImpl;

    /* make compatible to boost's graph_traits */
    typedef typename boost::graph_traits<GraphImpl>::vertex_descriptor	    vertex_descriptor;
    typedef typename boost::graph_traits<GraphImpl>::edge_descriptor        edge_descriptor;

    typedef typename boost::graph_traits<GraphImpl>::adjacency_iterator     adjacency_iterator;
    typedef typename boost::graph_traits<GraphImpl>::out_edge_iterator      out_edge_iterator;
    typedef typename boost::graph_traits<GraphImpl>::in_edge_iterator       in_edge_iterator;
    typedef typename boost::graph_traits<GraphImpl>::vertex_iterator        vertex_iterator;
    typedef typename boost::graph_traits<GraphImpl>::edge_iterator          edge_iterator;

    typedef typename boost::graph_traits<GraphImpl>::directed_category      directed_category;
    typedef typename boost::graph_traits<GraphImpl>::edge_parallel_category edge_parallel_category;
    typedef typename boost::graph_traits<GraphImpl>::traversal_category     traversal_category;
    typedef typename boost::graph_traits<GraphImpl>::vertices_size_type     vertices_size_type;
    typedef typename boost::graph_traits<GraphImpl>::edges_size_type        edges_size_type;
    typedef typename boost::graph_traits<GraphImpl>::degree_size_type       degree_size_type;

    typedef VertexProp   vertex_property_type;

    static inline vertex_descriptor null_vertex() {
        return boost::graph_traits<GraphImpl>::null_vertex();
    }

    /* convenient abbreviations */
    typedef std::pair<adjacency_iterator, adjacency_iterator> adjacency_vertex_range_t;
    typedef std::pair<out_edge_iterator, out_edge_iterator> out_edge_range_t;
    typedef std::pair<in_edge_iterator, in_edge_iterator> in_edge_range_t;
    typedef std::pair<vertex_iterator, vertex_iterator> vertex_range_t;
    typedef std::pair<edge_iterator, edge_iterator> edge_range_t;

    Graph() : _directed(true) { }
    Graph(const int size) : _graph(size), _directed(true) { }
    virtual ~Graph() { }

    void clear() { _graph.clear(); }
    vertex_descriptor add_vertex() { return boost::add_vertex(_graph); }
    void clear_vertex(const vertex_descriptor& v) {
        boost::clear_vertex(v, _graph); // remove all edges from/to v
    }
    // Be careful, changes _all_ node indexes!
    void remove_vertex(const vertex_descriptor& v) {
        boost::clear_vertex(v, _graph); // remove all edges from/to v
        boost::remove_vertex(v, _graph);// remove v from graph
    }
    edge_descriptor add_edge(const vertex_descriptor& v1,
			     const vertex_descriptor& v2) {
        return boost::add_edge(v1, v2, _graph).first;
    }
    void remove_edge(const edge_descriptor& e) {
        boost::remove_edge(e, _graph);
    }

    /**
     * Is v2 an out-neighbor of v2?
     */
    bool is_out_neighbor(const vertex_descriptor& v1, const vertex_descriptor& v2) const {
        assert(v1 < num_vertices() && v2 < num_vertices());
        return boost::is_adjacent(_graph, v1, v2);
    }
    /**
     * Is v2 an in-neighbor of v2?
     */
    bool is_in_neighbor(const vertex_descriptor& v1, const vertex_descriptor& v2) const {
        in_edge_iterator ie, ieend;
        for(boost::tie(ie, ieend) = in_edges(v1); ie != ieend; ++ie)
            if(v2 == source(*ie))
                return true;
        return false;
    }
    vertex_descriptor target(const edge_descriptor& e) const {
        return boost::target(e, _graph);
    }
    vertex_descriptor source(const edge_descriptor& e) const {
        return boost::source(e, _graph);
    }
    out_edge_range_t out_edges(vertex_descriptor& v) {
        return boost::out_edges(v, _graph);
    }
    out_edge_range_t out_edges(const vertex_descriptor& v) const {
        return boost::out_edges(v, _graph);
    }
    in_edge_range_t in_edges(vertex_descriptor& v) {
        return boost::in_edges(v, _graph);
    }
    in_edge_range_t in_edges(const vertex_descriptor& v) const {
        return boost::in_edges(v, _graph);
    }

    const VertexProp& property(const vertex_descriptor &v) const {
        return _graph[v];
    }
    VertexProp& property(const vertex_descriptor &v) {
        return _graph[v];
    }
    const EdgeProp& property(const edge_descriptor &e) const {
        return _graph[e];
    }
    EdgeProp& property(const edge_descriptor &e) {
        return _graph[e];
    }

    vertex_range_t vertices() const { return boost::vertices(_graph); }
    adjacency_vertex_range_t adjacent_vertices(const vertex_descriptor& v) const {
        return boost::adjacent_vertices(v, _graph);
    }
    unsigned int num_vertices() const { return boost::num_vertices(_graph); }
    unsigned int num_edges() const { return boost::num_edges(_graph); }

    unsigned int out_degree(const vertex_descriptor& v) const { return boost::out_degree(v, _graph); }
    unsigned int in_degree(const vertex_descriptor& v) const { return boost::in_degree(v, _graph); }
    unsigned int degree(const vertex_descriptor& v) const { return boost::degree(v, _graph); }

    Graph& operator=(const Graph &other) {
        _graph = other._graph;
        return *this;
    }

    typedef VertexProp VertexProperty;
    struct VertexPropertyConverter {
	const char * key_name() const {
	    if (typeid(VertexProp) == typeid(NoProperty))
		return NULL;
	    return "sequoia_node_data";
	}
	void set(Graph &g, const vertex_descriptor& vertex, std::string value) {
	    std::stringstream s(value);
	    VertexProperty newval;
	    s >> newval;
	    g.property(vertex) = newval;
	}
	std::string get(const Graph &g, const vertex_descriptor& vertex) {
	    std::stringstream s;
	    s << g.property(vertex);
	    return s.str();
	}
    };

    typedef EdgeProp EdgeProperty;
    struct EdgePropertyConverter {
	const char * key_name() const {
	    if (typeid(EdgeProp) == typeid(NoProperty))
		return NULL;
	    return "sequoia_edge_data";
	}
	void set(Graph &g, const edge_descriptor& edge, std::string value) {
	    std::stringstream s(value);
	    EdgeProperty newval;
	    s >> newval;
	    g.property(edge) = newval;
	}
	std::string get(const Graph &g, const edge_descriptor& edge) {
	    std::stringstream s;
	    s << g.property(edge);
	    return s.str();
	}
    };

    /*
     * Not for public use.  They are required for GraphFactory/GraphPrinter,
     * which cannot be declared friends since this for some reason
     * leads to strange "ambigous decleration" errors (due identical
     * declarations here and in LabeledGraph/TreeDecomposition)
     */
    GraphImpl & impl() { return _graph; }
    const GraphImpl & impl() const { return _graph; }
    void adapt(const UGraph& other);
    // UGraph as_undirected() const;
private:
    GraphImpl _graph;
    bool _directed;
};

} // namespace

#include "graph.hpp"

#endif	/* _GRAPH_H */
