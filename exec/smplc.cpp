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
#include <IRVisualizerPass.hpp>
#include <RemapPass.hpp>
#include <CSEPass.hpp>

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
        std::vector<std::reference_wrapper<IR::Pass>> irPasses;
        std::unordered_map<std::string, std::shared_ptr<IR::BlockEntry>> blockMap;

        PrintPass printPass;
        if(arguments.parserDebug){
            parserPasses.emplace_back(printPass);
        }
        IRGeneratorPass irGeneratorPass(blockMap);
        RemapPass remapPass;
        std::optional<IRVisualizerPass> irVisualizerPass;
        CSEPass csePass;

        if(!arguments.parseOnly){
            parserPasses.emplace_back(irGeneratorPass);
            irPasses.emplace_back(remapPass);
            if(arguments.withCSE){
                irPasses.emplace_back(csePass);
            }
            if(!arguments.irVisualizeFile.empty()){
                irPasses.emplace_back(irVisualizerPass.emplace(arguments.irVisualizeFile));
            }
        }

        // Parse
        if(!Parser::Computation(sourceFile, parserPasses).parse() || Logger::errorCount() > 0){
            printLogs(arguments.inputFiles[0]);
            return -1;
        }

        // Run IR passes
        for(std::reference_wrapper<IR::Pass> pass : irPasses){
            pass.get().traverse(blockMap);
            if(Logger::errorCount() > 0){
                printLogs(arguments.inputFiles[0]);
                return -2;
            }
        }
        printLogs(arguments.inputFiles[0]);
    }catch(Exception& err){
        ColorPrint::fatal(err.what());
    }
    return 0;
}
