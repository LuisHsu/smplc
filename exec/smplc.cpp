/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

#include <Source.hpp>
#include <Exception.hpp>
#include <Parser.hpp>
#include <Logger.hpp>
#include <PrintPass.hpp>
#include <EmitWasmPass.hpp>
#include <GenWasm.hpp>

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
    try{
        // Check argument
        ArgParse arguments(argc, argv);
        // Create source instance
        std::ifstream fileIn(arguments.inputFiles[0]);
        if(!fileIn.is_open()){
            ColorPrint::fatal("can't open input file");
            return -1;
        }
        Source sourceFile(fileIn);

        // Create passes
        std::vector<std::reference_wrapper<Parser::Pass>> parserPasses;

        PrintPass printPass;
        if(arguments.parserDebug){
            parserPasses.emplace_back(printPass);
        }

        EmitWasmPass wasmPass;
        if(!arguments.parseOnly){
            parserPasses.emplace_back(wasmPass);
        }

        // Parse
        if(!Parser::Computation(sourceFile, parserPasses).parse() || Logger::errorCount() > 0){
            printLogs(arguments.inputFiles[0]);
            return -1;
        }

        // Output wasm
        GenWasm::wrapper(arguments.outputFile);
        GenWasm::binary(arguments.outputFile, wasmPass.module);

        // Print logs
        printLogs(arguments.inputFiles[0]);
    }catch(Exception& err){
        ColorPrint::fatal(err.what());
    }
    return 0;
}
