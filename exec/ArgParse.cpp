/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "ArgParse.hpp"

ArgParse::ArgParse(int argc, char const *argv[]):
    parserDebug(false)
{
    for(int i = 1; i < argc; ++i){
        if(std::string(argv[i]) == "--parser_debug"){
            parserDebug = true;
        }else{
            inputFiles.emplace_back(argv[i]);
        }
    }
}