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
#ifndef SEQUOIA_STRUCTURES_TREEDECOMPOSITION_H
#define	SEQUOIA_STRUCTURES_TREEDECOMPOSITION_H

#include "graph.h"
#include "bag.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <boost/graph/adjacency_list_io.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>

namespace sequoia {

class TreeDecomposition : public Graph<Bag> {
public:
    typedef Graph<Bag> Base;

    TreeDecomposition() : _root(null_vertex()) { }
    TreeDecomposition(int size)
    : Base(size), _root(null_vertex()) { }

    vertex_descriptor root() const { return _root; }
    void root(const vertex_descriptor& r) { _root = r; }
    const Bag* bag(const vertex_descriptor& v) const { return &property(v); }
    Bag* bag(const vertex_descriptor& v) { return &property(v); }
    void make_nice();
    size_t width() const {
        size_t w = 0;
        vertex_iterator v, vend;
        for (boost::tie(v, vend) = vertices(); v != vend; ++v)
            w = std::max(bag(*v)->width(), w);
        return w;
    }
    
    struct VertexPropertyConverter {
	const char * key_name() const { return "sequoia_bag"; }
	void set(TreeDecomposition &tdc, const vertex_descriptor &vertex,
		 const std::string &line) {
	    std::vector<std::string> parts;
	    boost::split(parts, line, boost::is_any_of(" ,\t\n\r"));
	    for(unsigned int j = 0; j < parts.size(); ++j) {
		if (parts[j].size() == 0) continue;
		int id = boost::lexical_cast<int>(parts[j].c_str());
		tdc.bag(vertex)->add(id);
	    }
	}
	std::string get(const TreeDecomposition &tdc, const vertex_descriptor &vertex) {
	    std::stringstream s;
	    const Bag* bag = tdc.bag(vertex);
	    for (unsigned int i = 0; i < bag->width(); ++i)
		s << (i > 0 ? ", " : "") << bag->get(i);
	    return s.str();
	}
    };

private:
    template <class Graph> friend class TreeDecompositionCheck;
    vertex_descriptor _root;
};

} // namepace

#endif // SEQUOIA_STRUCTURES_TREEDECOMOSITION_H

