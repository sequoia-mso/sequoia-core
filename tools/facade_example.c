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
#include <stdio.h>
#include <stdlib.h>

#include <sequoia.h>

#define N 30 // try 40 for a no-instance

int main(int argc, char** argv) {
    sequoia_t f = sequoia_create();
    int vertices[N], i, j, u, v;

    /* create random graph on N vertices */
    for (i = 0; i < N; i++)
	vertices[i] = sequoia_add_vertex(f);
    for (u = 0; u < N; u++)
	for (v = u+1; v < N; v++)
	    if (rand() % 10 == 0)
		if (sequoia_add_edge(f, vertices[u], vertices[v]))
		    return EXIT_FAILURE;

    // three colorability
    char res = sequoia_solve(f,
	"threecol(R, B) := All x ((x notin R or x notin B) and "
	    "All y (~adj(x,y) or"
	    "((x notin R or y notin R) and "
	    " (x notin B or y notin B) and "
	    " ((x in R) or (x in B) or (y in R) or (y in B)))))",
	"Witness");

    if (res == -1)
	return EXIT_FAILURE;
    if (res == 0) {
	printf("Graph is NOT three-colorable.\n");
	return EXIT_SUCCESS;
    }
    printf("Graph is three-colorable, here is a coloring:\n");
    int *p = sequoia_solution(f);

    // first position contains number of sets to follow
    int n_sets = *p++;
    for (i = 0; i < n_sets; i++) {
	// first position in each set specifies number of elements to follow
	int n_elements = *p++;
	printf("Color %d:", i);
	for (j = 0; j < n_elements; j++)
	    printf(" %d", *p++);
	printf("\n");
    }
    sequoia_destroy(f);
    return EXIT_SUCCESS;
}

