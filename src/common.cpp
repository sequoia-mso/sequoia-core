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
#include "common.h"

#include <string.h>

namespace sequoia {

std::ostream & tab_prefix(int i) {
    std::string ret = "";
    while(i-- > 0) ret += "\t";
    return std::cout << ret;
}

const char * str2char(std::string s) {
  int len = s.size() + 1;
  char* res = new char[len];
  ::strncpy(res, s.c_str(), len);
  return res;
}

}
