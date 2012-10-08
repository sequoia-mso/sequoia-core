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
#ifndef SEQUOIA_H
#define	SEQUOIA_H

#ifdef	__cplusplus
extern "C"
{
#endif

typedef void*	sequoia_t;    // facade handle type

/**
 * Create a new facade object with an empty graph.
 * @return a handle to the newly created facade object
 */
sequoia_t sequoia_create();
/**
 * Destroy a facade object.
 */
void sequoia_destroy(sequoia_t f);

/**
 * Add a vertex to the facade object
 * @param f facade object
 * @return handle of the newly created vertex, or -1 on error
 */
int sequoia_add_vertex(sequoia_t f);
/**
 * Add an edge between the two specified vertices u, v.  This is a directed
 * graph, so make sure you add back edges for undirected graphs.
 * @param f facade object
 * @param u vertex
 * @param v vertex
 * @return true if creation was successful, or false else
 */
int sequoia_add_edge(sequoia_t f, int u, int v);

/**
 * Solve the problem specified by the formula on the graph.
 * @param f facade object
 * @param formula	the MSO formula
 * @param evaluation	the evaluation to use.  Refer to the documentation
 *			for possible values.
 * @return -1	on error, 0 when no solution was found, 1 when a solution was found.
 */

int sequoia_solve(sequoia_t f, const char *formula, const char *evaluation);

/**
 * Return the solution if there is one.
 * Prerequisite:  sequoia_solve() returned 1.
 * Refer to the documentation on info how the return value can be interpreted.
 * @param f facade object
 */
int * sequoia_solution(sequoia_t f);

#ifdef	__cplusplus
}
#endif

#endif	/* SEQUOIA_H */
