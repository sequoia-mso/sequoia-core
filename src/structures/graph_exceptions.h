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
#ifndef SEQUOIA_GRAPH_EXCEPTIONS_H
#define	SEQUOIA_GRAPH_EXCEPTIONS_H

#include <exception>
#include <string>

namespace sequoia {

struct graph_exception : public std::exception {
    graph_exception() { }
    virtual ~graph_exception() throw() { }
    virtual const char* what() const throw() = 0;
};

struct graph_syntax_error : public graph_exception {
    graph_syntax_error(std::string e) : errmsg(e) { }
    ~graph_syntax_error() throw() { }
    const char* what() const throw() { return errmsg.c_str(); }
    std::string errmsg;
};

struct treedecomposition_invalid : public graph_exception {
    treedecomposition_invalid(std::string e) : errmsg(e) { }
    ~treedecomposition_invalid() throw() { }
    const char* what() const throw() { return errmsg.c_str(); }
    std::string errmsg;
};

struct graph_file_error : public graph_exception {
    graph_file_error(std::string f)
    : filename(f) {
	errmsg = std::string("Failed to open file: ") + filename;
    }
    graph_file_error(std::string f, std::string e) : filename(f), errmsg(e) { }
    ~graph_file_error() throw() { }
    const char* what() const throw() {
	return errmsg.c_str();
    }
    std::string filename;
    std::string errmsg;
};

struct graph_unknown_file_format : public graph_exception {
    graph_unknown_file_format(std::string ext) {
	errmsg = std::string("Unknown file format: ") + ext;
    }
    graph_unknown_file_format() {
	errmsg = std::string("Cannot detect file format.");
    }
    ~graph_unknown_file_format() throw() { }
    const char* what() const throw() {
	return errmsg.c_str();
    }
    std::string errmsg;
};

} // namespace

#endif	/* SEQUOIA_GRAPH_EXCEPTIONS_H */

