/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */
#include <iostream>
#include <fstream>

#include <Source.hpp>
#include <Exception.hpp>

#include "ColorPrint.hpp"

int main(int argc, char const *argv[]){
    // Check argument
    if(argc < 2){
        ColorPrint::fatal("no input files");
    }else if(argc > 2){
        ColorPrint::fatal("only support one input file now");
    }
    // Create source instance
    std::ifstream fileIn(argv[1]);
    Source sourceFile(fileIn);

    // Parse
    try{
        // TODO: parse
    }catch(Exception& err){
        ColorPrint::fatal(err.what());
    }
    return 0;
}
