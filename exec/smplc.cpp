/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <functional>

#include <Source.hpp>
#include <Exception.hpp>
#include <Parser.hpp>
#include <Logger.hpp>
#include <PrintPass.hpp>
#include <IRGeneratorPass.hpp>

#include "ColorPrint.hpp"
#include "ArgParse.hpp"

void printLogs(std::string fileName){
    Logger::dump([&](unsigned int lineNum, LogLevel level, std::string msg){
        switch (level){
        case LogLevel::Error :
            ColorPrint::error(fileName, lineNum, msg.c_str());
            break;
        case LogLevel::Warning :
            ColorPrint::warning(fileName, lineNum, msg.c_str());
            break;
        case LogLevel::Info :
            ColorPrint::info(fileName, lineNum, msg.c_str());
            break;
        }
    });
}

int main(int argc, char const *argv[]){
    // Check argument
    ArgParse arguments(argc, argv);
    if(arguments.inputFiles.size() < 1){
        ColorPrint::fatal("no input file");
        return -1;
    }else if(arguments.inputFiles.size() > 1){
        ColorPrint::fatal("only support one input file now");
        return -1;
    }
    // Create source instance
    std::ifstream fileIn(arguments.inputFiles[0]);
    if(!fileIn.is_open()){
        ColorPrint::fatal("can't open input file");
        return -1;
    }
    Source sourceFile(fileIn);

    // Create parser passes
    std::vector<std::reference_wrapper<Parser::Pass>> parserPasses;
    std::optional<PrintPass> printPass;
    if(arguments.parserDebug){
        parserPasses.emplace_back(printPass.emplace());
    }
    std::optional<IRGeneratorPass> irGeneratorPass;
    if(!arguments.parseOnly){
        parserPasses.emplace_back(irGeneratorPass.emplace());
    }

    // Start
    try{
        // Parse
        if(!Parser::Computation(sourceFile, parserPasses).parse() || Logger::errorCount() > 0){
            printLogs(arguments.inputFiles[0]);
            return -1;
        }
    }catch(Exception& err){
        ColorPrint::fatal(err.what());
    }
    printLogs(arguments.inputFiles[0]);
    return 0;
}
