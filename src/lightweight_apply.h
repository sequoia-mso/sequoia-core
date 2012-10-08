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
#ifndef SEQUOIA_LEIGHTWEIGHT_APPLY_H
#define SEQUOIA_LEIGHTWEIGHT_APPLY_H

namespace sequoia {

namespace internal {

template <typename Complex>
class LightWeightApply {
public:
    typedef typename Complex::argument_type argument_type;
    LightWeightApply<Complex>(Complex *complex) : _complex(complex) { }
    void operator()(const argument_type &arg) const { (*_complex)(arg); }
private:
    Complex *_complex;
};

} // namespace internal

template <typename Complex>
typename internal::LightWeightApply<Complex>
lightweight_apply(Complex *complex) {
    return internal::LightWeightApply<Complex>(complex);
}

} // namespace sequoia

#endif //  SEQUOIA_LEIGHTWEIGHT_APPLY_H