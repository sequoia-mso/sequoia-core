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
#ifndef SEQUOIA_SEQUOIA_APP_H
#define SEQUOIA_SEQUOIA_APP_H

#include "sequoia_solver.h"

namespace sequoia {

class SequoiaSolverApp {
public:
    SequoiaSolverApp();
    void init(int argc, char **argv);
    int run();
    void usage();
private:
    const char *_evaluation;
    const char *_graph;
    const char *_treedecomposition;
    const char *_formula;
    const char *_solution;
    const char *_cache_size;
    bool _2flag;
    int _threads;
};

} // namespace

#endif // SEQUOIA_SEQUOIA_APP_H