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
#ifndef GRAPH_FILE_FORMATS_H
#define	GRAPH_FILE_FORMATS_H

namespace sequoia {

struct GraphFileFormats {
    enum Format {
	UNKNOWN
	,GRAPHML
	,LEDA
	,DIMACS
#if HAVE_GRAPHVIZ
	,GRAPHVIZ
#endif
    }; 

    static Format
    guess(const std::string &ext) {
	if (ext == ".graphml")
	    return GRAPHML;
	if (ext == ".leda")
	    return LEDA;
	if (ext == ".dimacs")
	    return DIMACS;
#if HAVE_GRAPHVIZ
	if (ext == ".gv" || ext == ".graphviz")
	    return GRAPHVIZ;
#endif
	return UNKNOWN;
    }

    static const char *
    name(Format f) {
	switch (f) {
	case GRAPHML: return "GraphML";
	case LEDA: return "LEDA";
	case DIMACS: return "DIMACS";
#if HAVE_GRAPHVIZ
	case GRAPHVIZ: return "GraphViz";
#endif
	default: return "Unknown";
	}
    }
};

} // namespace

#endif	/* GRAPH_FILE_FORMATS_H */

