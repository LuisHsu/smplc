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
        std::string arg(argv[i]);
        if(arg == "--parser_debug"){
            parserDebug = true;
        }else if(arg == "--parse_only"){
            parseOnly = true;
        }else if(arg == "-o" || arg == "--output"){
            if((i + 1) >= argc){
                throw Exception("no output file prefix after '-o' or '--output' option");
            }
            outputFile = argv[++i];
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
    // Output file
    if(outputFile.empty()){
        outputFile = "a";
    }
}