/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Logger.hpp>

Logger::Logger():
    numInfo(0), numWarning(0), numError(0)
{}

Logger Logger::logger = Logger();

void Logger::put(LogLevel level, std::string msg){
    logger.logs.emplace(level, msg);
    switch (level){
    case LogLevel::Error :
        logger.numError += 1;
        break;
    case LogLevel::Info :
        logger.numInfo += 1;
        break;
    case LogLevel::Warning :
        logger.numWarning += 1;
        break;
    }
}

unsigned int Logger::infoCount(){
    return logger.numInfo;
}

unsigned int Logger::warningCount(){
    return logger.numWarning;
}

unsigned int Logger::errorCount(){
    return logger.numError;
}

void Logger::dump(std::function<void(LogLevel, std::string)> callback){
    while(!logger.logs.empty()){
        std::pair<LogLevel, std::string>& entry = logger.logs.front();
        callback(entry.first, entry.second);
        logger.logs.pop();
    }
    logger.numError = 0;
    logger.numInfo = 0;
    logger.numWarning = 0;
}