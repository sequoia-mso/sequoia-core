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
 * Reads a file in DIMACS CNF format and writes a corresponding graph file
 * with vertex and edge labels.
 * @author Alexander Langer
 */
#include "structures/labeled_graph.h"
#include "structures/graph_printer.h"
#include <iostream>
#include <boost/regex.hpp>

using namespace sequoia;

void usage(char *name) {
    std::cerr << "Usage: " << name << " <infile> <outfile>" << std::endl;
    std::exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
    if(argc < 3) {
        std::cerr << "Error:  Not enough arguments given" << std::endl;
        usage(argv[0]);
    }
    const char *infile = argv[1];
    const char *outfile = argv[2];

    if (strcmp(infile, outfile) == 0) {
        std::cerr << "Error:  infile = outfile" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    boost::filesystem::path p(infile);
    if (!boost::filesystem::exists(p)) {
        std::cerr << "Error: " << infile
            << ": No such file or directory." << std::endl;
        return EXIT_FAILURE;
        
    }

    std::fstream in(infile, std::fstream::in);
    if (!in.good()) {
        std::cerr << "Error opening file: " << infile << std::endl;
        return EXIT_FAILURE;
        
    }
    std::cout << "Reading file " << infile << std::endl;

    typedef LabeledGraph::vertex_descriptor vertex_t;
    LabeledGraph g;

    std::vector<vertex_t> vertices_by_index;
    size_t current_clause = 0, num_v, num_c;
    int var_label, clause_label, pos_label, neg_label;

    while (!in.eof()) {
        char buffer[256];
        in.getline(buffer, 256);
	//std::cerr << "line is: " << buffer << std::endl;
        std::string s(buffer);
        if (s.size() == 0) continue;
        std::vector<std::string> v;
        boost::split(v, buffer, boost::is_any_of(" \t\n\r"));
        if (v[0] == "c")
            continue;
        if (v[0] == "p") {
	    int cnfmarker = 1;
	    while (cnfmarker < v.size() && v[cnfmarker].length() == 0) cnfmarker++;
	    int varmarker = cnfmarker + 1;
	    while (varmarker < v.size() && v[varmarker].length() == 0) varmarker++;
	    int clausemarker = varmarker + 1;
	    while (clausemarker < v.size() && v[clausemarker].length() == 0) clausemarker++;
	    if(v[cnfmarker] != "cnf" || clausemarker >= v.size()) {
                std::cerr << "expected DIMACS CNF file, but got line "
                    << buffer << std::endl;
                return EXIT_FAILURE;
            }
	    num_v = boost::lexical_cast<std::size_t > (v[varmarker].c_str());
	    num_c = boost::lexical_cast<std::size_t > (v[clausemarker].c_str());
            //std::cout << "Variablen: " << num_v << ", Clauses: " << num_c << std::endl;
            g = LabeledGraph(num_v + num_c);
            var_label = g.create_label("Var");
            clause_label = g.create_label("Clause");
            pos_label = g.create_label("Pos");
            neg_label = g.create_label("Neg");

            std::copy(g.vertices().first,
                      g.vertices().second,
                      std::back_inserter(vertices_by_index));
            current_clause = num_v;
            for(size_t v = 0; v < num_v; v++)
                g.vertex_add_label(vertices_by_index[v], var_label);
            for(size_t v = num_v; v < num_v+num_c; v++)
                g.vertex_add_label(vertices_by_index[v], clause_label);
        } else {
            assert(current_clause > 0);
            //std::cout << "has buffer: " << buffer << " for clause " << current_clause << std::endl;
            for (size_t i = 0; i < v.size(); i++) {
	    	if (v[i].length() == 0) continue; // multiple white space characters
                int var = boost::lexical_cast<int>(v[i].c_str());
                if (var == 0) {
                    current_clause++;
                    continue;
                }
                vertex_t var_vertex;
                vertex_t clause_vertex = vertices_by_index[current_clause];
                int label = 0;
                if (var > 0) {
                    var_vertex = vertices_by_index[var-1];
                    label = pos_label;
                } else {
                    var_vertex = vertices_by_index[-var-1];
                    label = neg_label;
                }
                //std::cout << "Add: " << clause_vertex << " / " << var_vertex << " " << g.label(label) << std::endl;
                LabeledGraph::edge_descriptor e = g.add_edge(var_vertex, clause_vertex);
                g.edge_add_label(e, label);
            }
        }
    }

    assert(num_v + num_c == current_clause);

    std::cout << "Writing file " << outfile << std::endl;

    try {
	GraphPrinter<LabeledGraph>::write_graph(argv[2], g);
    } catch (const std::exception &e) {
	std::cerr << "Error writing graph: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
