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
#include <assert.h>

#include "atomar_game_factory.h"
#include <boost/scoped_ptr.hpp>

namespace sequoia {

template <typename TFormula, typename TIndex, typename TReturnGame>
void LeafGameFactory::visit_impl(const TFormula* f) {
    assert(f != NULL);
    DEBUG({
        tab_prefix(_level) << "Building for Formula: "
                << f->toString() << std::endl;
    });
    TReturnGame* returngame = new TReturnGame(f);
    DEBUG(returngame->level(_level));
    typename TFormula::subformula_iterator s, send;
    for (boost::tie(s, send) = f->subformulas(); s != send; ++s) {
	const Formula* subf = *s;
        boost::scoped_ptr<const Assignment_f> alpha(_alpha->clone());
	const TIndex *index = prepare_index(f, subf, alpha);
    	DEBUG(tab_prefix(_level) << TOSTRING(index) << std::endl);
        LeafGameFactory factory(alpha.get());
        DEBUG(factory.level(_level+1));
    	subf->accept(&factory);
        const MCGame_f* sub = factory.get();
    	DEBUG(tab_prefix(_level) << TOSTRING(index) << " --> " << sub->get()->toString());
        const MCGame_f* tmpgame = returngame->add_subgame(index, sub);
    	if (tmpgame != NULL) {
	    _game = tmpgame;
	    return;
	}
    }
    _game = returngame->minimize();
}

} // namespace
