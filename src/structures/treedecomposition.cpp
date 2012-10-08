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
#include "treedecomposition.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>

namespace sequoia {

class MakeNice {
    typedef TreeDecomposition::vertex_descriptor Vertex;
    typedef TreeDecomposition::edge_descriptor Edge;
    typedef TreeDecomposition::out_edge_iterator OutEdgeIterator;
    typedef TreeDecomposition::vertex_iterator VertexIterator;

    // assert that the leafs have empty bags
    static bool fix_root_leafs(TreeDecomposition &tdc, Vertex& v) {
        if (tdc.in_degree(v) == 0) {
#if 0
            if (tdc.bag(v)->width() > 0) {
                Vertex n = tdc.add_vertex();
                tdc.add_edge(n, v);
                tdc.root(n);
                return true;
            }
#endif
            tdc.root(v);
        }
        if (tdc.out_degree(v) == 0) {
            if (tdc.bag(v)->width() > 0) {
                Vertex n = tdc.add_vertex();
                tdc.add_edge(v, n);
                return true;
            }
        }
        return false;
    }

    // make tree binary
    static bool fix_binary(TreeDecomposition &tdc, Vertex& v) {
        if (tdc.out_degree(v) < 3)
            return false;
        // add two new bags that are identical and make them the sole
        // children of v
        Vertex first = tdc.add_vertex();
        Vertex second = tdc.add_vertex();
        *tdc.bag(first) = *tdc.bag(second) = *tdc.bag(v);

        int c = 0;
        // split all old children to these two bags evenly
        OutEdgeIterator o, oend;
        std::vector<Edge> remove;
        for (boost::tie(o, oend) = tdc.out_edges(v); o != oend; ++o) {
            Vertex parent = c++ % 2 == 0 ? first : second;
            tdc.add_edge(parent, tdc.target(*o));
            remove.push_back(*o);
        }
        tdc.add_edge(v, first);
        tdc.add_edge(v, second);
        for (unsigned int i = 0; i < remove.size(); ++i)
            tdc.remove_edge(remove[i]);
        return true;
    }

    // fix joins
    static bool fix_joins(TreeDecomposition &tdc, Vertex& v) {
        if (tdc.out_degree(v) != 2)
            return false;
        OutEdgeIterator o, oend;
        std::vector<Edge> remove(0);
        for (boost::tie(o, oend) = tdc.out_edges(v); o != oend; ++o) {
            Vertex child = tdc.target(*o);
            if (*tdc.bag(v) == *tdc.bag(child))
                continue;
            remove.push_back(*o);
        }
        if (remove.size() == 0)
            return false;
        for (unsigned int i = 0; i < remove.size(); ++i) {
            Vertex a = tdc.add_vertex();
            Edge e = remove[i];
            *tdc.bag(a) = *tdc.bag(v);
            tdc.add_edge(v, a);
            tdc.add_edge(a, tdc.target(e));
            tdc.remove_edge(e);
        }
        return true;
    }

    // fix introduce
    static bool fix_introduce_forget(TreeDecomposition &tdc, Vertex& v) {
        if (tdc.out_degree(v) != 1)
            return false;
        OutEdgeIterator o, oend;
        boost::tie(o, oend) = tdc.out_edges(v);
        assert(o != oend);
        Edge e = *o;
        Vertex child = tdc.target(e);

        Bag* mybag = tdc.bag(v);
        Bag* childbag = tdc.bag(child);

#if 0
        if (mybag->width() == childbag->width() && mybag->is_subset_of(childbag)) {
            assert(false);
            // child is useless, remove
            OutEdgeIterator o, oend;
            for (boost::tie(o, oend) = tdc.out_edges(child); o != oend; ++o) {
                tdc.add_edge(v, tdc.target(*o));
            }
            tdc.remove_vertex(child);
            return true;
        }
#endif

        // is valid introduce already?
        if (childbag->is_subset_of(*mybag) &&
            mybag->width() == childbag->width() + 1)
            return false;
        // is valid forget already?
        if (mybag->is_subset_of(*childbag) && 
            mybag->width() == childbag->width() - 1)
            return false;

        // add new intermediate vertex containing the intersection
        tdc.remove_edge(e);
        Vertex m = tdc.add_vertex();
        tdc.add_edge(v, m);
        tdc.add_edge(m, child);

        // get new handles for the bag, they have changed due to the new vertex
        mybag = tdc.bag(v);
        childbag = tdc.bag(child);

        Bag* medbag = tdc.bag(m);
        // first add the intersection of both vertices
        for (unsigned int i = 0; i < mybag->width(); ++i) {
            int v = mybag->get(i);
            if (childbag->member(v))
                medbag->add(v);
        }
        Bag* smaller = mybag;
        Bag* larger = childbag;
        if (mybag->width() > childbag->width()) {
            larger = mybag;
            smaller = childbag;
        }
        // now add one extra vertex
        if (smaller->is_subset_of(*larger)) {
            // find first vertex not in intersection and add
            for (unsigned int i = 0; i < larger->width(); ++i) {
                int v = larger->get(i);
                if (!smaller->member(v)) {
                    medbag->add(v);
                    break;
                }
            }
        }
        return true;
    }




    static bool fix_single_vertex(TreeDecomposition &tdc, Vertex& v) {
        if (fix_root_leafs(tdc, v))
            return true;
        if (fix_binary(tdc, v))
            return true;
        if (fix_joins(tdc, v))
            return true;
        if (fix_introduce_forget(tdc, v))
            return true;
        return false;
    }

  public:
    static void make_nice(TreeDecomposition& tdc) {
        bool changed = false;
        TreeDecomposition::vertex_descriptor last = *tdc.vertices().first;
        do {
            changed = false;
            VertexIterator t, tend;
            for (boost::tie(t, tend) = tdc.vertices(); t != tend; ++t) {
                Vertex v = *t;
                if (v < last) continue;
                last = v;
                if ((changed = fix_single_vertex(tdc, v)))
                     break;
            }
        } while (changed);
#if 0
        std::cout << "Resulting bags: " << std::endl;
        VertexIterator t, tend;
        for (boost::tie(t, tend) = tdc.vertices(); t != tend; ++t) {
            std::cout << *tdc.bag(*t) << std::endl;
        }
#endif
    }
};

void TreeDecomposition::make_nice() {
    MakeNice::make_nice(*this);
}

} // namespace


