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
#ifndef SEQUOIA_EXCEPTIONS_H
#define	SEQUOIA_EXCEPTIONS_H

#include <exception>
#include <string>

namespace sequoia {

struct sequoia_exception : public std::exception {
    sequoia_exception() { }
    virtual ~sequoia_exception() throw() { }
    virtual const char* what() const throw() = 0;
};

struct sequoia_usage_error : public sequoia_exception {
    sequoia_usage_error(std::string e) : errmsg(e) { }
    sequoia_usage_error(const char *e) : errmsg(std::string(e)) { }
    ~sequoia_usage_error() throw() { }
    const char* what() const throw() { return errmsg.c_str(); }
    std::string errmsg;
};

struct sequoia_internal_error : public sequoia_exception {
    sequoia_internal_error(std::string e) : errmsg(e) { }
    sequoia_internal_error(const char *e) : errmsg(std::string(e)) { }
    ~sequoia_internal_error() throw() { }
    const char* what() const throw() { return errmsg.c_str(); }
    std::string errmsg;
};

struct sequoia_formula_error : public sequoia_exception {
    sequoia_formula_error(std::string e) : errmsg(e) { }
    sequoia_formula_error(const char *e) : errmsg(std::string(e)) { }
    ~sequoia_formula_error() throw() { }
    const char* what() const throw() { return errmsg.c_str(); }
    std::string errmsg;
};

struct sequoia_file_error : public sequoia_exception {
    sequoia_file_error(std::string e) : errmsg(e) { }
    sequoia_file_error(const char *e) : errmsg(std::string(e)) { }
    ~sequoia_file_error() throw() { }
    const char* what() const throw() { return errmsg.c_str(); }
    std::string errmsg;
};

struct sequoia_incident_symbol_found : public sequoia_exception {
    sequoia_incident_symbol_found(const std::string &ident) {
	errmsg = std::string("Special symbol '") + ident + "' used as free variable.";
    }
    ~sequoia_incident_symbol_found() throw() { }
    const char* what() const throw() { return errmsg.c_str(); }
    std::string errmsg;
};

} // namespace

#endif	/* SEQUOIA_EXCEPTIONS_H */

