/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "ArgParse.hpp"

#include <Exception.hpp>

ArgParse::ArgParse(int argc, char const *argv[]):
    parserDebug(false), parseOnly(false)
{
    for(int i = 1; i < argc; ++i){
        if(std::string(argv[i]) == "--parser_debug"){
            parserDebug = true;
        }else if(std::string(argv[i]) == "--parse_only"){
            parseOnly = true;
        }else if(std::string(argv[i]) == "--visualize_ir"){
            if(++i < argc){
                irVisualizeFile = argv[i];
            }else{
                throw Exception("no output file for visualize IR");
            }
        }else{
            inputFiles.emplace_back(argv[i]);
        }
    }
    // Input files
    if(inputFiles.size() < 1){
        throw Exception("no input file");
    }else if(inputFiles.size() > 1){
        throw Exception("only support one input file now");
    }
}