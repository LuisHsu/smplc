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
#include <Logger.hpp>

#include "ColorPrint.hpp"

void printLogs(){
    Logger::dump([](LogLevel level, std::string msg){
        switch (level){
        case LogLevel::Error :
            ColorPrint::error(msg.c_str());
            break;
        case LogLevel::Warning :
            ColorPrint::error(msg.c_str());
            break;
        case LogLevel::Info :
            ColorPrint::error(msg.c_str());
            break;
        }
    });
}

int main(int argc, char const *argv[]){
    // Check argument
    if(argc < 2){
        ColorPrint::fatal("no input file");
        return -1;
    }else if(argc > 2){
        ColorPrint::fatal("only support one input file now");
        return -1;
    }
    // Create source instance
    std::ifstream fileIn(argv[1]);
    if(!fileIn.is_open()){
        ColorPrint::fatal("can't open input file");
        return -1;
    }
    Source sourceFile(fileIn);

    // Parse
    try{
        if(!Parser::Computation(sourceFile).parse() || Logger::errorCount() > 0){
            printLogs();
            return -1;
        }
    }catch(Exception& err){
        ColorPrint::fatal(err.what());
    }
    printLogs();
    return 0;
}
