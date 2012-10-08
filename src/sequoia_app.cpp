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
#include "exceptions.h"
#include "sequoia_app.h"
#include "structures/graph_printer.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_TBB
#include <tbb/task_scheduler_init.h>
#endif

namespace sequoia {

SequoiaSolverApp::SequoiaSolverApp() :
    _evaluation(NULL),
    _graph(NULL),
    _treedecomposition(NULL),
    _formula(NULL),
    _cache_size(NULL),
    _solution(NULL),
    _2flag(false),
    _threads(0) {
}

void SequoiaSolverApp::init(int argc, char **argv) {
    int ch;
    while ((ch = getopt(argc, argv, "c:f:e:g:t:T:2s:")) != -1) {
        switch (ch) {
        case 'c':
            _cache_size = optarg;
            break;
        case 'f':
            _formula = optarg;
            break;
        case 'g':
            _graph = optarg;
            break;
        case 't':
            _treedecomposition = optarg;
            break;
        case 'e':
            _evaluation = optarg;
            break;
        case '2':
            _2flag = true;
            break;
        case 's':
	    _solution = optarg;
	    break;
        case 'T':
            _threads = atoi(optarg);
            break;
        case '?':
            usage();
            exit(EXIT_SUCCESS);
        default:
            std::cerr << "Error: Unknown argument: " << argv[optind] << std::endl;
            usage();
        }
    }
    if(argc - optind > 0) {
        std::cerr << "ERROR:  Unknown command line argument: " << argv[optind] << std::endl;
        usage();
	exit(EXIT_FAILURE);
    }

    if (_formula == NULL) {
        std::cerr << "ERROR:  No formula specified" << std::endl;
        usage();
	exit(EXIT_FAILURE);
    }

    if (_graph == NULL) {
        std::cerr << "ERROR:  No input graph specified" << std::endl;
        usage();
	exit(EXIT_FAILURE);
    }


    if (_evaluation == NULL) {
        std::cerr << "ERROR:  No evaluation specified" << std::endl;
        usage();
	exit(EXIT_FAILURE);
    }
}

int SequoiaSolverApp::run() {
    std::cout << "This is " << PACKAGE_STRING << "." << std::endl;
#ifdef HAVE_TBB
    tbb::task_scheduler_init task_sched_init(-2);
    if (_threads > 0) {
	//if (_threads > 4) _threads = 4;
    	std::cout << "Limiting to " << _threads << " threads." << std::endl;
	task_sched_init.initialize(_threads);
    } else {
	int num = tbb::task_scheduler_init::default_num_threads();
	if (num > 4) {
    	    std::cout << "Threading library wants to schedule " << num << " threads, limiting to 4." << std::endl;
		task_sched_init.initialize(4);
	} else {
	    task_sched_init.initialize(-1);
        }
    }
#endif
    SequoiaSolver solver;

    try {
	solver.load_graph(_graph);
        solver.create_incidence_graph(_2flag);
	if (_treedecomposition != NULL)
	    solver.load_treedecomposition(_treedecomposition);

	solver.load_formula(_formula);
	solver.load_evaluation(std::string(_evaluation));

    } catch (const sequoia_incident_symbol_found &e) {
	std::cerr << "Sequoia fatal error: " << e.what() << std::endl;
        std::cerr << "Special symbols 'Vertex' and 'Edge' must not be used as free variables." << std::endl;
        std::cerr << "Use the -2 command line option to use the incidence graph model." << std::endl;
	exit(EXIT_FAILURE);
    } catch (const std::exception &e) {
	std::cout << "Sequoia fatal error: " << e.what() << std::endl;
	exit(EXIT_FAILURE);
    }
    
    if (_cache_size != NULL) {
        long s = atol(_cache_size);
        solver.cache_size(s);
    }

    solver.solve();

    std::cout << "Solution:" << std::endl;
    if(solver.has_solution()) {
        const void *sol = solver.solution();
        const Evaluation* eval = solver.evaluation();
	eval->output_solution(std::cout, sol);
        std::cout << std::endl;

	try {
	    if (_solution != NULL) {
		const GraphStructure *graph = solver.solution_graph();
		GraphPrinter<LabeledGraph>::write_graph(_solution, *graph->graph());
		delete graph;
	    }
	} catch (const std::exception &e) {
	    std::cout << "Error writing solution file: " << e.what() << std::endl;
	    exit(EXIT_FAILURE);
	}
    } else { // !has_solution()
        std::cout << "NONE" << std::endl;
    }
    return EXIT_SUCCESS;
}

/**
 * Print usage() and exit with failure.
 */
void SequoiaSolverApp::usage() {
    std::cerr << "Usage: " << std::endl;
    std::cerr << "\t -? \t\t\tshow this help and exit" << std::endl;
    std::cerr << "\t -f <formula>\t\tMSO formula" << std::endl;
    std::cerr << "\t -e <Bool,MinCard,MaxCard,MinCardSet,MaxCardSet,Witness>" << std::endl;
    std::cerr << "\t\t\t\tevaluation to use" << std::endl;
    std::cerr << "\t -g <LEDA file>\t\tinput graph" << std::endl;
    std::cerr << "\t -t <LEDA file>\t\ttreedecomposition" << std::endl;
    std::cerr << "\t -2 \t\t\tuse the incidence graph model" << std::endl;
    std::cerr << "\t -s <solution graph>\twrite annotated graph to file" << std::endl;
    std::cerr << "Performance options:" << std::endl;
#ifdef HAVE_TBB
    std::cerr << "\t -T <num>\t\tlimit to <num> parallel threads" << std::endl;
#endif
    std::cerr << "\t -c <size>>\t\tcache expensive computations using caches of size <size>" << std::endl;
}

} // namespace
