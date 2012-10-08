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
#ifndef GRAPH_HPP
#define	GRAPH_HPP

#include "graph.h"

#include <algorithm>
#include <boost/tuple/tuple.hpp>

namespace sequoia {

template <typename VertProp, typename EdgeProp>
void
Graph<VertProp, EdgeProp>::adapt(const UGraph &other) {
    *this = Graph<VertProp,EdgeProp>(boost::num_vertices(other));
    this->_directed = false;
    
    typedef typename boost::graph_traits<UGraph> UGraphTraits;
    typename UGraphTraits::vertex_iterator v, vend;
    typename UGraphTraits::out_edge_iterator e, eend;
    typename UGraphTraits::vertex_descriptor s, t;

    for (boost::tie(v, vend) = boost::vertices(other); v != vend; v++) {
	for (boost::tie(e, eend) = boost::out_edges(*v, other); e != eend; e++) {
	    s = boost::source(*e, other);
	    t = boost::target(*e, other);
	    if (t < s) continue;
	    this->add_edge(s, t);
	}
    }
};

#if 0 // unused
template <typename VertProp>
UGraph
Graph<VertProp>::as_undirected() const {
    UGraph g(num_vertices());
    vertex_iterator v, vend;
    out_edge_iterator e, eend;
    for (boost::tie(v, vend) = this->vertices(); v != vend; v++) {
	for (boost::tie(e, eend) = this->out_edges(*v); e != eend; e++)
	    boost::add_edge(this->source(*e), this->target(*e), g);
    }
    return g;
};
#endif


}

#endif	/* GRAPH_HPP */

