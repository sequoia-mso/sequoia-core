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
#ifndef SEQUOIA_TREEDECOMPOSITION_CHECK_H
#define SEQUOIA_TREEDECOMPOSITION_CHECK_H

#include "common.h"
#include "structures/treedecomposition.h"
#include "graph_exceptions.h"

#include <list>
#include <set>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/iterator/filter_iterator.hpp>

namespace sequoia {

class bag_has_vertex {
  public:
    bool operator()(const TreeDecomposition::vertex_descriptor& node) {
        return _tdc->bag(node)->member(_vertex);
    }
    void vertex(const int vertex) { _vertex = vertex; }
    void tdc(const TreeDecomposition* tdc) { _tdc = tdc; }
  private:
    int _vertex;
    const TreeDecomposition *_tdc;
};

using namespace boost;

template <typename TimeMap>
class dfs_time_visitor : public default_dfs_visitor {
    typedef typename property_traits<TimeMap>::value_type T;
  public:
    dfs_time_visitor(TimeMap dmap, TimeMap fmap, T & t) :
        m_dtimemap(dmap), m_ftimemap(fmap), m_time(t) { }
    template <typename Vertex, typename Graph>
    void discover_vertex(Vertex u, const Graph & g) const {
        put(m_dtimemap, u, m_time++);
    }
    template < typename Vertex, typename Graph >
    void finish_vertex(Vertex u, const Graph & g) const {
        put(m_ftimemap, u, m_time++);
    }
    TimeMap m_dtimemap;
    TimeMap m_ftimemap;
    T & m_time;
};

template <class Graph> class TreeDecompositionCheck {
    typedef typename boost::graph_traits<Graph> GraphTraits;
    typedef typename GraphTraits::vertex_descriptor Vertex;
    typedef typename GraphTraits::edge_descriptor Edge;
    typedef typename GraphTraits::vertex_iterator GraphVertexIterator;
    typedef typename GraphTraits::out_edge_iterator GraphOutEdgeIterator;

    //typedef typename GraphTraits::size_type size_type;
    typedef size_t size_type;
    typedef typename std::list<Vertex> VertexList;
    typedef typename VertexList::iterator VertexListIterator;
    typedef typename std::set<Vertex> VertexSet;
    typedef typename VertexSet::iterator VertexSetIterator;

    typedef typename TreeDecomposition::vertex_descriptor TreeNode;
    typedef typename TreeDecomposition::vertex_iterator TreeVertexIterator;
    typedef typename TreeDecomposition::out_edge_iterator TreeOutEdgeIterator;

    const Graph& g;
    const TreeDecomposition& tdc;

  public:
    TreeDecompositionCheck(const Graph& ag, const TreeDecomposition& atdc) :
        g(ag), tdc(atdc) { }

    bool nice_at(const TreeNode& t) {
        // root is empty
        if (tdc.in_degree(t) == 0) {
            if (t != tdc.root())
		throw treedecomposition_invalid(std::string(
			"Node with in_degree 0 detected that is not the root"));
#if 0
            if (tdc.bag(t)->width() != 0) {
                std::cerr << "Node " << t << "is the root, but the bag is not empty" << std::endl;
                return false;
            }
#endif
        }
        // Leafs are empty
        if (tdc.out_degree(t) == 0)
            if (tdc.bag(t)->width() != 0)
		throw treedecomposition_invalid(std::string(
			"Non-empty leaf found"));
        if (tdc.out_degree(t) > 2)
	    throw treedecomposition_invalid(std::string(
		    "Node of degree > 2 found"));

        if (tdc.out_degree(t) == 1) {
            TreeOutEdgeIterator o, oend;
            boost::tie(o, oend) = tdc.out_edges(t);
            TreeNode c = tdc.target(*o);
            const Bag* mybag = tdc.bag(t);
            const Bag* childbag = tdc.bag(c);
            if (mybag->is_subset_of(*childbag) && mybag->width() + 1 == childbag->width())
                return true;
            if (childbag->is_subset_of(*mybag) && childbag->width() + 1 == mybag->width())
                return true;
	    throw treedecomposition_invalid(std::string("Invalid node of degree one found"));
            return false;
        }
        TreeOutEdgeIterator o, oend;
        for (boost::tie(o, oend) = tdc.out_edges(t); o != oend; ++o) {
            TreeNode c = tdc.target(*o);
            if (*tdc.bag(t) != *tdc.bag(c))
		throw treedecomposition_invalid(std::string("Invalid join node found"));
        }
        return true;
    }

    /**
     * Test if this is a valid nice tree decomp
     */
    bool is_nice() {
        TreeVertexIterator t, tend;
        for (boost::tie(t, tend) = tdc.vertices(); t != tend; ++t)
            if (nice_at(*t) == false)
                return false;
        return true;
    }

    /**
     * Tests whether this is a valid tree decomposition
     * @return
     */
    bool valid() {
        //const BoostGraph &bg = tdc.graph();
        //print_graph(bg);
        if (!is_tree()) return false;
        if (!all_vertices_contained()) return false;
        if (!all_edges_contained()) return false;
        if (!has_connectivity_property()) return false;
        return true;
    }
    std::set<TreeNode> marked;
    bool is_tree() {
        return is_tree(tdc.root());
    }
    bool is_tree(TreeNode t) {
        // mark current node
        marked.insert(t);
        // traverse subtrees
        TreeOutEdgeIterator c, cend;
        for (boost::tie(c, cend) = tdc.out_edges(t); c != cend; ++c) {
            if (marked.count(tdc.target(*c)) > 0) {
                std::stringstream s;
		s << "Loop " << tdc.source(*c) << "-" << tdc.target(*c) << " detected";
		throw treedecomposition_invalid(s.str());
            }
            if (is_tree(tdc.target(*c)) == false)
                return false;
        }
        return true;
    }

    bool all_vertices_contained() {
        GraphVertexIterator v, vend;
        for (boost::tie(v, vend) = g.vertices(); v != vend; ++v) {
            if (!is_contained(*v)) {
                std::stringstream s;
		s << "Vertex " << *v << " not contained in any bag!";
		throw treedecomposition_invalid(s.str());
            }
        }
        return true;
    }
    bool is_contained(const Vertex& v) {
        TreeVertexIterator t, tend;
        for (boost::tie(t, tend) = tdc.vertices(); t != tend; ++t) {
            if (tdc.bag(*t)->member(v))
                return true;
        }
        return false;
    }

    bool all_edges_contained() {
        GraphVertexIterator v, vend;
        for (boost::tie(v, vend) = g.vertices(); v != vend; ++v) {
            GraphOutEdgeIterator n, nend;
            for (boost::tie(n, nend) = g.out_edges(*v); n != nend; ++n) {
                if (!is_contained(*n)) {
                    Edge e = *n;
		    std::stringstream s;
                    s << "Edge " << g.source(e) << "->" << g.target(e)
                        << " not contained in any bag!";
		    throw treedecomposition_invalid(s.str());
                }
            }
        }
        return true;
    }
    bool is_contained(const Edge& e) {
        TreeVertexIterator t, tend;
        for (boost::tie(t, tend) = tdc.vertices(); t != tend; ++t) {
            if (tdc.bag(*t)->member(g.source(e)) &&
                tdc.bag(*t)->member(g.target(e)))
                return true;
        }
        return false;
    }

    typedef TreeDecomposition::GraphImpl BoostGraph;
    typedef boost::graph_traits<BoostGraph>::vertex_descriptor BoostVertex;
    typedef boost::graph_traits<BoostGraph>::vertex_iterator VertexIter;
    typedef boost::filter_iterator<bag_has_vertex, VertexIter> FilterIter;

    BoostGraph create_copy(const BoostGraph& orig, FilterIter& f, FilterIter& fend) {
        std::map<BoostVertex, BoostVertex> vertexmap;
        BoostGraph newg;
        FilterIter a;
        for (a = f; a != fend; ++a) {
            vertexmap[*a] = boost::add_vertex(newg);
        }
        for (a = f; a != fend; ++a) {
            FilterIter b = a;
            for (; b != fend; ++b) {
                if (a == b) continue;
                if (boost::is_adjacent(orig, *a, *b) ||
                    boost::is_adjacent(orig, *b, *a)) {
                    boost::add_edge(vertexmap[*a], vertexmap[*b], newg);
                    boost::add_edge(vertexmap[*b], vertexmap[*a], newg);
                }
            }
        }
        return newg;
    }

    bool has_connectivity_property() {
        const BoostGraph &bg = tdc.impl();
#if 0
        std::cout << "Original Graph: " << std::endl;
        print_graph(bg);
#endif

        //boost::filtered_graph<BoostGraph, positive_edge_weight<EdgeWeightMap> > fg(g, filter);

        GraphVertexIterator v, vend;
        bag_has_vertex predicate;
        predicate.tdc(&tdc);
        for (boost::tie(v, vend) = g.vertices(); v != vend; ++v) {

            // create subgraph of all nodes with bags containing v
            predicate.vertex(*v);
            std::pair<VertexIter, VertexIter> verts = boost::vertices(bg);
            FilterIter filter_iter_first(predicate, verts.first, verts.second);
            FilterIter filter_iter_last(predicate, verts.second, verts.second);
            BoostGraph copyg = create_copy(bg, filter_iter_first,
                                           filter_iter_last);
#if 0
            print_graph(copyg);
#endif
            // do dfs on the copied graph to check whether it's connected
            std::vector<size_t> dtime(num_vertices(copyg));
            std::vector<size_t> ftime(num_vertices(copyg));
            size_t t = 0;
            dfs_time_visitor<size_t*> vis(&dtime[0], &ftime[0], t);
            depth_first_search(copyg, visitor(vis));

            if (ftime[0] != boost::num_vertices(copyg) * 2 - 1) {
		std::stringstream s;
		s << "Subgraph of bags containing " << *v
			<< " is not connected!" << std::endl;
		throw treedecomposition_invalid(s.str());
            }
        }
        return true;
    }
};

} // namespace

#endif // SEQUOIA_TREEDECOMPOSITION_CHECK_H