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
#include "incidence_graph.h"

namespace sequoia {

void 
create_incidence_graph(const LabeledGraph &g, LabeledGraph &outg,
                       TreeDecomposition *tdc) {
    // create new graph
    outg = LabeledGraph(g.num_vertices());

    // add existing labels
    for (unsigned int i = 0; i < g.num_labels(); i++)
        outg.create_label(g.label(i));
    // add predefined labels for vertices and edges
    int Vid = outg.create_label(std::string("Vertex"));
    int Eid = outg.create_label(std::string("Edge"));

    // convert the graph
    LabeledGraph::vertex_iterator it, itend;
    LabeledGraph::vertex_descriptor current;
    for (boost::tie(it, itend) = g.vertices(); it != itend; it++) {
        LabeledGraph::vertex_descriptor v = *it;

        // add the vertex label to each vertex
        outg.vertex_add_label(v, Vid);
        // add all previous labels
        for (unsigned int i = 0; i < g.num_labels(); i++)
            if (g.vertex_has_label(v, i))
                outg.vertex_add_label(v, i);

        LabeledGraph::out_edge_iterator eit, eitend;
        for (boost::tie(eit, eitend) = g.out_edges(v); eit != eitend; eit++) {
	    LabeledGraph::vertex_descriptor u = g.target(*eit);

	    // add the "edge" type vertex
	    current = outg.add_vertex();
            outg.vertex_add_label(current, Eid);
            outg.add_edge(v, current);
	    outg.add_edge(current, u);

	    // add edge labels as vertex labels
	    for (unsigned int i = 0; i < g.num_labels(); i++)
		if (g.edge_has_label(*eit, i))
		    outg.vertex_add_label(current, i);

	    // create new edge in tdc for this edge vertex
	    if (tdc != NULL) {
		// find bag that contains v and u
		Bag *bag = NULL;
		TreeDecomposition::vertex_descriptor t;
		TreeDecomposition::vertex_iterator tit, titend;
		for (boost::tie(tit, titend) = tdc->vertices(); tit != titend; tit++) {
		    t = *tit;
		    if (tdc->bag(t)->member(u) && tdc->bag(t)->member(v)) {
			bag = tdc->bag(t);
			break;
		    }
		}
		assert(bag != NULL);

		TreeDecomposition::vertex_descriptor new_t = tdc->add_vertex();
		tdc->add_edge(t, new_t);
		Bag *new_bag = tdc->bag(new_t);
		new_bag->add(u);
		new_bag->add(v);
		new_bag->add(current);
	    } // tdc != NULL
        }
    }
}

void
create_adjacency_graph(const LabeledGraph &g, LabeledGraph &outg) {
    outg = LabeledGraph();

    int Vid = g.label_id(std::string("Vertex"));
    int Eid = g.label_id(std::string("Edge"));
    assert(Vid != -1 && Eid != -1);
    
    std::map<int, int> labels;
    // add existing labels, except Vertex and Edge
    for (unsigned int i = 0; i < g.num_labels(); i++)
	if (i != Vid && i != Eid) {
	    labels[i] = outg.create_label(g.label(i));
	    std::cout << "label: " << g.label(i) << " has new id: " << labels[i] << std::endl;
	}

    std::map<LabeledGraph::vertex_descriptor, LabeledGraph::vertex_descriptor> old2new;

    // convert the graph

    // first convert all vertices to fill the old2new map
    LabeledGraph::vertex_iterator it, itend;
    for (boost::tie(it, itend) = g.vertices(); it != itend; it++) {
        LabeledGraph::vertex_descriptor v = *it;
	if (g.vertex_has_label(v, Vid)) {
	    // Vertex found
	    assert(!g.vertex_has_label(v, Eid));

	    LabeledGraph::vertex_descriptor new_v = outg.add_vertex();
	    old2new[v] = new_v;

	    // add all previous labels
	    for (unsigned int i = 0; i < g.num_labels(); i++)
		if (i != Vid && i != Eid)
		    if (g.vertex_has_label(v, i))
			outg.vertex_add_label(new_v, labels[i]);

	}
    }

    // now add all edges
    for (boost::tie(it, itend) = g.vertices(); it != itend; it++) {
        LabeledGraph::vertex_descriptor v = *it;
	if (g.vertex_has_label(v, Eid)) {
	    // Edge found
	    assert(!g.vertex_has_label(v, Vid));
	    
	    // find source vertex
	    LabeledGraph::in_edge_iterator sit, sitend;
	    boost::tie(sit, sitend) = g.in_edges(v);
	    assert(sit != sitend);
	    LabeledGraph::vertex_descriptor s = g.source(*sit);

	    // find target vertex
	    LabeledGraph::out_edge_iterator tit, titend;
	    boost::tie(tit, titend) = g.out_edges(v);
	    assert(tit != titend);
	    LabeledGraph::vertex_descriptor t = g.target(*tit);

	    LabeledGraph::edge_descriptor new_e = outg.add_edge(old2new[s], old2new[t]);

	    // add all previous labels
	    for (unsigned int i = 0; i < g.num_labels(); i++)
		if (i != Vid && i != Eid)
		    if (g.vertex_has_label(v, i))
			outg.edge_add_label(new_e, labels[i]);
	}
    }
}

} // namespace