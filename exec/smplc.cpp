/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */
#include <iostream>
#include <fstream>

#include <Source.hpp>
#include <Exception.hpp>
#include <Parser.hpp>

#include "ColorPrint.hpp"

int main(int argc, char const *argv[]){
    // Check argument
    if(argc < 2){
        ColorPrint::fatal("no input files");
        return -1;
    }else if(argc > 2){
        ColorPrint::fatal("only support one input file now");
        return -1;
    }
    // Create source instance
    std::ifstream fileIn(argv[1]);
    Source sourceFile(fileIn);

    // Parse
    try{
        if(!Parser::Computation(sourceFile).parse()){
            return -1;
        }
    }catch(Exception& err){
        ColorPrint::fatal(err.what());
    }
    return 0;
}
