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
#ifndef SEQUOIA_APPLY_INTRODUCE_H
#define SEQUOIA_APPLY_INTRODUCE_H

#include "apply_dynstep.h"
#include "apply_game_introduce.h"
#include "sequoia_table.h"

namespace sequoia {

template <typename Solver>
class ApplyIntroduce :
    public ApplyDynStep<
	ApplyIntroduce<Solver>,
	ApplyGameIntroduce<Solver>,
	Solver
    > {
public:
    typedef ApplyDynStep<
	ApplyIntroduce<Solver>,
	ApplyGameIntroduce<Solver>,
	Solver
    > Base;
    typedef std::vector<SequoiaTable*> SequoiaNodeTables;
    ApplyIntroduce<Solver>(Solver *solver,
			   const TreeDecomposition::vertex_descriptor& node,
	                   const TreeDecomposition::vertex_descriptor& child)
      : Base(solver, node, child) { }
protected:
    friend class ApplyDynStep<
	ApplyIntroduce<Solver>,
	ApplyGameIntroduce<Solver>,
	Solver>;
    void spec_init();
    const PointMove* update_pointmove_impl(unsigned int i) const;
private:
    const PointMove* create_tmove() const;
    BitSet _neighbors, _labels;
    ApplyIntroduce<Solver>(const ApplyIntroduce<Solver> &); // forbid
};

} // namespace

#include "apply_introduce.hpp"

#endif // SEQUOIA_APPLY_INTRODUCE_H