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
#ifndef SEQUOIA_STRUCTURES_GRAPH_STRUCTURE_H
#define SEQUOIA_STRUCTURES_GRAPH_STRUCTURE_H

#include "labeled_graph.h"
#include "../logic/structure.h"

namespace sequoia {

class GraphVocabulary : public Vocabulary {
public:
    GraphVocabulary() {
        add_symbol(new Symbol("adj", 2, false, 0));
    }
};

/**
 * An adaptor of LabeledGraph to expose it as a structure in the sense
 * of mathematical logic.
 * 
 * TODO:  Not really, we just provide graph() to access its underlying
 *        graph directly!
 */
class GraphStructure : public Structure {
public:
    GraphStructure(const GraphVocabulary *vocabulary, const LabeledGraph* graph)
    : Structure(vocabulary), _graph(graph) { }
    GraphStructure(const LabeledGraph* graph);
    virtual ~GraphStructure() {
	delete _graph;
    }
    bool has_label(const LabeledGraph::vertex_descriptor& v,
                   const UnarySymbol* label) const {
        int label_id = _graph->label_id(label->identifier());
        if (label_id == -1) return false;
        return _graph->vertex_has_label(v, label_id);
    }
    unsigned int num_labels() const {
        return _graph->num_labels();
    }
    const UnarySymbol* label(int l) const {
        return vocabulary()->unary_symbol(l);
    }
    const LabeledGraph* graph() const { return _graph; }
private:
    friend class GraphStructureFactory;
    void graph(const LabeledGraph* graph) { this->_graph = graph; }
    const LabeledGraph* _graph;
};

} // namespace

#endif // SEQUOIA_STRUCTURES_GRAPH_STRUCTURE_H