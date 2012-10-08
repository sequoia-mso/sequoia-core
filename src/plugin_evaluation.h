/*
 * This file is part of the Sequoia MSO Solver.
 * 
 * Copyright 2012 Fred Grossmann
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
 * @author Fred Grossmann
 */
#include <dlfcn.h>      // dlopen

namespace sequoia {

// TODO make this an evaluation that loads the actual impleentation from a plugin

void
destroy() {
    void (*destroy)(Evaluation*);
    destroy = (void(*)(Evaluation*))dlsym(_handle, "destroy_object");
    char* dlsym_error= dlerror();
    destroy( _evaluation );
    if (dlsym_error)
	std::cerr << "Cannot load symbol destroy: " << dlsym_error << '\n';

}

void load_evaluation(const char* filename) {
    std::stringstream fileLocation;
    //fileLocation << "../evaluations/" << filename << ".so";
    fileLocation << "./" << filename << ".so";
    
    const std::string& tmp = fileLocation.str();
    const char* cstr = tmp.c_str();

    void* handle = dlopen(cstr, RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot load evaluation library: " << filename
                << " because: " << dlerror() << std::endl;
        throw "Cannot load evaluation";
    }
    // reset errors
    dlerror();
    
    Evaluation* (*create)();
    //void (*destroy)(Evaluation*);
    create = (Evaluation* (*)())dlsym(handle, "create_object");
    const char* dlsym_error = dlerror();

    if (dlsym_error) {
        std::cerr << "Cannot load symbol create: " << dlsym_error << '\n';
        throw "Cannot load evaluation";
    }
    
    Evaluation* myClass = (Evaluation*) create();
    myClass->init(_terminal_symbols, _free_unary_symbols);
    _evaluation = myClass;
    _handle = handle;
}

} // namespace
