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
#ifndef SEQUOIA_LABELED_GRAPH_H
#define	SEQUOIA_LABELED_GRAPH_H

#include "common.h"
#include "graph.h"
#include "../unordered_defs.h"

#include "graph_factory.h"

#include <algorithm>
#include <sstream>
#include <vector>

namespace sequoia {

struct SetMembership {
    typedef SEQUOIA_UNORDERED_SET<int> Set;
    Set sets;
    bool contains(int k) const { return sets.count(k) > 0; }
    void add(int k) { sets.insert(k); }
    void remove(int k) { sets.erase(k); }
};

class LabeledGraph : public Graph<SetMembership, SetMembership> {
public:
    typedef SetMembership VertexProperty;
    typedef SetMembership EdgeProperty;
    typedef Graph<VertexProperty, EdgeProperty> Base;

    LabeledGraph() { }
    LabeledGraph(int size) : Base(size) { }

    unsigned int num_labels() const { return _labels.size(); }
    unsigned int create_label(const std::string& label) {
        int s = _labels.size();
        _labels[label] = s;
	_label_names.push_back(label);
        return s;
    }
    int label_id(const std::string& label) const {
        LabelIndexMap::const_iterator it = _labels.find(label);
        if (it == _labels.end()) return -1;
        return it->second;
    }
    const std::string& label(unsigned int id) const {
        assert(id < num_labels());
	return _label_names[id];
    }

    bool vertex_has_label(const vertex_descriptor& v, int id) const {
	return this->property(v).contains(id);
    }
    void vertex_add_label(const vertex_descriptor &v, int id) {
	this->property(v).add(id);
    }
    void vertex_remove_label(const vertex_descriptor &v, int id) {
	this->property(v).remove(id);
    }
    void vertex_add_label(const vertex_descriptor &v, const std::string& label) {
        int id = label_id(label);
        if (id == -1) id = create_label(label);
	vertex_add_label(v, id);
    }
    void vertex_remove_label(const vertex_descriptor &v, const std::string& label) {
        int id = label_id(label);
        if (id == -1) {
            std::cerr << "Error removing unknown label " << label << " for vertex " << v << std::endl;
	    assert(false);
        }
	vertex_remove_label(v, id);
    }

    bool edge_has_label(const edge_descriptor &e, int id) const {
	return this->property(e).contains(id);
    }
    void edge_add_label(const edge_descriptor &e, int id) {
	this->property(e).add(id);
    }
    void edge_remove_label(const edge_descriptor &e, int id) {
	this->property(e).remove(id);
    }
    void edge_add_label(const edge_descriptor &e, const std::string& label) {
        int id = label_id(label);
        if (id == -1) id = create_label(label);
	edge_add_label(e, id);
    }
    void edge_remove_label(const edge_descriptor &e, const std::string& label) {
        int id = label_id(label);
        if (id == -1) {
            std::cerr << "Error removing unknown label " << label << " for edge " << e << std::endl;
	    assert(false);
        }
	edge_remove_label(e, id);
    }

    struct VertexPropertyConverter {
	const char * key_name() const { return "sequoia_vertex_sets"; }
	void set(LabeledGraph &graph, const vertex_descriptor &vertex,
	         const std::string &val) {
	    std::vector<std::string> parts;
	    boost::split(parts, val, boost::is_any_of(" ,\t\n\r"));
	    for(unsigned int j = 0; j < parts.size(); ++j) {
		if (parts[j].size() == 0) continue;
		graph.vertex_add_label(vertex, parts[j]);
	    }
	}
	std::string get(const LabeledGraph &graph, const vertex_descriptor &vertex) {
	    std::stringstream s;
	    int count = 0;
	    for (unsigned int i = 0; i < graph.num_labels(); i++)
		if (graph.vertex_has_label(vertex, i))
		    s << (count++ > 0 ? "," : "") << graph.label(i);
	    return s.str();
	}
    };

    struct EdgePropertyConverter {
	const char * key_name() const { return "sequoia_edge_sets"; }
	void set(LabeledGraph &graph, const edge_descriptor &edge,
	         const std::string &val) {
	    std::vector<std::string> parts;
	    boost::split(parts, val, boost::is_any_of(" ,\t\n\r"));
	    for(unsigned int j = 0; j < parts.size(); ++j) {
		if (parts[j].size() == 0) continue;
		graph.edge_add_label(edge, parts[j]);
	    }
	}
	std::string get(const LabeledGraph &graph, const edge_descriptor &edge) {
	    std::stringstream s;
	    int count = 0;
	    for (unsigned int i = 0; i < graph.num_labels(); i++)
		if (graph.edge_has_label(edge, i))
		    s << (count++ > 0 ? "," : "") << graph.label(i);
	    return s.str();
	}
    };

private:
    typedef SEQUOIA_UNORDERED_MAP<std::string, int> LabelIndexMap;
    LabelIndexMap _labels;
    std::vector<std::string> _label_names;
};




}; // namespace

#endif // SEQUOIA_LABELED_GRAPH_H
