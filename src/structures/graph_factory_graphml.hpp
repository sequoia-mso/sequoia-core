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
#ifndef SEQUOIA_STRUCTURES_GRAPH_FACTORY_GRAPHML_HPP
#define SEQUOIA_STRUCTURES_GRAPH_FACTORY_GRAPHML_HPP

#include "graph.h"
#include "graph_exceptions.h"

#include <vector>
#include <sstream>
#include <iostream>
#include <string>
#include <utility>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/graph/copy.hpp>

#include <boost/shared_ptr.hpp>

namespace sequoia {


namespace internal {

void
read_graphml(std::istream& in, boost::shared_ptr<boost::mutate_graph> g);

template <typename Graph>
class MutableGraph : public boost::mutate_graph {
    typedef typename boost::graph_traits<Graph> GraphTraits;
    typedef typename GraphTraits::vertex_descriptor vertex_descriptor;
    typedef typename GraphTraits::edge_descriptor edge_descriptor;
public:
    MutableGraph<Graph>(Graph &graph, boost::dynamic_properties& properties) 
    : _graph(graph), _properties(properties) { }

    void set_vertex_property(const std::string& name, boost::any vertex,
			     const std::string& value, const std::string& value_type) {
	vertex_descriptor v = boost::any_cast<vertex_descriptor>(vertex);
#if 0
	std::cout << "about to set vertex property: " << name << " for vertex "
		<< v << " to '" << value << "', where value_type = '" << value_type << "'" << std::endl;
#endif
	boost::put(name, _properties, v, value);
    }

    bool is_directed() const {
	return boost::is_convertible<
		typename GraphTraits::directed_category,
		boost::directed_tag
	    >::value;
    }

    boost::any do_add_vertex() { return boost::add_vertex(_graph); }

    std::pair<boost::any,bool> do_add_edge(boost::any source, boost::any target) {
	vertex_descriptor s = boost::any_cast<vertex_descriptor>(source);
	vertex_descriptor t = boost::any_cast<vertex_descriptor>(target);
	return boost::add_edge(s, t, _graph);
    }

    void
    set_graph_property(const std::string& name,
		       const std::string& value,
		       const std::string& value_type) {
#if 0
	std::cout << "about to set graph property: " << name
		<< " to '" << value << "', where value_type = '" << value_type << "'" << std::endl;
#endif

    }

    void
    set_edge_property(const std::string& name,
		      boost::any edge,
		      const std::string& value,
		      const std::string& value_type) {
	edge_descriptor e = boost::any_cast<edge_descriptor>(edge);
#if 0
	std::cout << "about to set edge property: " << name << " for edge "
		<< e << " to '" << value << "', where value_type = '" << value_type << "'" << std::endl;
#endif
	boost::put(name, _properties, e, value);
    }

private:
    Graph &_graph;
    boost::dynamic_properties &_properties;
};

template <typename Graph>
boost::shared_ptr<MutableGraph<Graph> >
make_mutable(Graph &g, boost::dynamic_properties &p) {
    return boost::shared_ptr<MutableGraph<Graph> >(new MutableGraph<Graph>(g, p));
}

} // namespace

template <typename Graph>
void GraphFactory<Graph>::load_graphml(Graph &g, std::istream& in,
				       boost::dynamic_properties& properties) {
    g.clear();
    try {
	bool undirected_input = false;
	try {
	    internal::read_graphml(in,internal::make_mutable(g.impl(), properties));
	    return;
	} catch (typename boost::undirected_graph_error &ex) {
	    undirected_input = true;
	}
	if (undirected_input) {
	    in.seekg(0UL);
	    UGraph tmp;
	    internal::read_graphml(in, internal::make_mutable(tmp, properties));
	    g.adapt(tmp);
	}
    } catch (boost::parse_error e) {
	throw graph_syntax_error(std::string(e.what()));
    }
}


} // namespace

#endif // SEQUOIA_STRUCTURES_GRAPH_FACTORY_GRAPHML_HPP