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
#ifndef SEQUOIA_MINDEGREE_HEURISTIC_H
#define SEQUOIA_MINDEGREE_HEURISTIC_H

#include "structures/treedecomposition.h"

#include "structures/graph_printer.h"

#include <iostream>
#include <list>
#include <set>

namespace sequoia {

template<typename Graph>
class MinDegreeHeuristic {
    typedef GraphPrinter<Graph> GP; // for debugging
    typedef typename boost::graph_traits<Graph> GraphTraits;
    typedef typename GraphTraits::vertex_descriptor Vertex;
    typedef typename GraphTraits::edge_descriptor Edge;
    //typedef typename GraphTraits::size_type size_type;
    typedef size_t size_type;
    typedef typename std::list<Vertex> VertexList;
    typedef typename VertexList::iterator VertexListIterator;
    typedef typename std::set<Vertex> VertexSet;
    typedef typename VertexSet::iterator VertexSetIterator;

    typedef typename TreeDecomposition::vertex_descriptor TreeNode;

    std::vector<VertexList*> buckets;
    size_type minimum_degree;
    std::vector<Vertex> vertices;
    std::vector<VertexSet*> neighbors;
    size_t num_vertices;
    size_t degree_bound;
    std::vector<TreeNode> tree_nodes;
  public:
    MinDegreeHeuristic(const Graph& g) : _graph(g) {
        num_vertices = _graph.num_vertices();
	degree_bound = num_vertices * 2;

	//GP::write_graph(std::cout, GraphFileFormats::LEDA, _graph);

        buckets.resize(degree_bound);
        neighbors.resize(num_vertices);
        vertices.resize(num_vertices);
        tree_nodes.resize(num_vertices);

        for(unsigned int i = 0; i < num_vertices; ++i)
            neighbors[i] = new VertexSet();
        for(unsigned int i = 0; i < degree_bound; ++i)
            buckets[i] = new VertexList();

        typename GraphTraits::vertex_iterator v, vend;
        for (boost::tie(v,vend) = _graph.vertices(); v != vend; ++v) {
            size_type degree = _graph.degree(*v);
            buckets[degree]->push_front(*v);
        }
    }
    virtual ~MinDegreeHeuristic() {
        for(unsigned int i = 0; i < num_vertices; ++i)
            delete neighbors[i];
        for(unsigned int i = 0; i < degree_bound; ++i) 
            delete buckets[i];
    }

    TreeDecomposition* get() { return tdc; }

    void update_bucket(const Vertex& v, size_t degree, size_t new_degree) {
	if (find(buckets[degree]->begin(), buckets[degree]->end(), v) == buckets[degree]->end()) {
            std::cerr << "Error:  Cannot find vertex " << v << " in bucket; does the graph have multi-edges or self-loops??" << std::endl;
	    throw;
	}
        buckets[degree]->remove(v);
        buckets[new_degree]->push_front(v);
        minimum_degree = std::min(minimum_degree, new_degree);
    }

    void make_clique(VertexSet* neighbors) {
        VertexSetIterator n = neighbors->begin();
        VertexSetIterator nend = neighbors->end();
        for (; n != nend; ++n) {
            size_type d = _graph.degree(*n);
            for(VertexSetIterator n2 = n; n2 != nend; ++n2) {
                if (*n == *n2) continue;
                if (_graph.is_in_neighbor(*n, *n2)) continue;
                if (_graph.is_out_neighbor(*n, *n2)) continue;
                size_type d2 = _graph.degree(*n2);
                _graph.add_edge(*n, *n2);
                update_bucket(*n, d, d+1);
                update_bucket(*n2, d2, d2+1);
                ++d;
            }
        }
    }

    void compute() {
        if (_graph.num_vertices() == 0) {
	    tdc = new TreeDecomposition();
	    tdc->add_vertex();
	    return;
        }


	/* the eleminatin order */
        size_type current_order = _graph.num_vertices() - 1;

        minimum_degree = 0;
        while (1) {
            VertexList* current_set = buckets[minimum_degree];
            while (current_set->size() == 0)
                current_set = buckets[++minimum_degree];

            VertexListIterator first = current_set->begin();
            Vertex node = *first;
            current_set->erase(first);

            assert(minimum_degree == _graph.degree(node));

            // save node and its neighborhood
            vertices[current_order] = node;
            VertexSet *neighbors = this->neighbors[current_order];
            typename GraphTraits::out_edge_iterator oe, oeend;
	    // adjust the buckets
            for (boost::tie(oe,oeend) = _graph.out_edges(node);
                 oe != oeend; ++oe) {
                Vertex neigh = _graph.target(*oe);
                neighbors->insert(neigh);
                size_type degree = _graph.degree(neigh);
                update_bucket(neigh, degree, degree - 1);
            }
	    // now do the same for in-neighbors, but be careful
	    // with in-neighbors that are also out-neighbors
            typename GraphTraits::in_edge_iterator ie, ieend;
            for (boost::tie(ie,ieend) = _graph.in_edges(node);
                 ie != ieend; ++ie) {
                Vertex neigh = _graph.source(*ie);
                neighbors->insert(neigh);
                size_type degree = _graph.degree(neigh);
                int out_neigh_off = 0;
                if (_graph.is_out_neighbor(node, neigh))
                    out_neigh_off = 1;
                update_bucket(neigh, degree - out_neigh_off,
                              degree - out_neigh_off - 1);
            }

            // take vertex out of graph
            _graph.clear_vertex(node);
            // make the neighborhood a clique
            make_clique(neighbors);

            if (current_order == 0) // found all vertices
                break;
            --current_order;
        }

        // Now reconstruct the tree decomposition from the elemination ordering.
        tdc = new TreeDecomposition();

        // First traverse in reverse order of elimination and
        // find the bags that contain the neighborhood-clique of
        // the eleminated vertex.
        for(unsigned int i = 0; i < num_vertices; ++i) {
            Vertex v = vertices[i];
            VertexSet* neighs = this->neighbors[i];
            tree_nodes[i] = tdc->add_vertex();
            Bag* bag = tdc->bag(tree_nodes[i]);

            // add neighborhood to bag
            VertexSetIterator n = neighs->begin();
            VertexSetIterator nend = neighs->end();
            for (; n != nend; ++n)
                bag->add(*n);

            if(i == 0) {
                tdc->root(tree_nodes[i]);
            } else {
                // find bag that contains this bag as a subset
                unsigned int j = i;
                while(--j) {
                    Bag* other = tdc->bag(tree_nodes[j]);
                    if (bag->is_subset_of(*other)) break;
                    if (j == 0) assert(false); // must not happen
                }
                assert(i != j);
                tdc->add_edge(tree_nodes[j], tree_nodes[i]);
            }
            bag->add(v);
        }
    }

  private:
    Graph _graph;
    TreeDecomposition* tdc;
};

} // namespace

#endif // SEQUOIA_MINDEGREE_HEURISTIC_H
