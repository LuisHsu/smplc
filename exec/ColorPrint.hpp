#ifndef SMPLC_ColorMessage_DEF
#define SMPLC_ColorMessage_DEF

#include <iostream>
#include <string>

namespace ColorPrint{
    
void fatal(std::string fileName, unsigned int lineNum, const char* msg){
    std::cerr << "\x1b[1msmplc: " << fileName + ":" << lineNum << " \x1b[91mfatal error:\x1b[0m " << msg << std::endl;
}
void fatal(const char* msg){
    std::cerr << "\x1b[1msmplc: \x1b[91mfatal error:\x1b[0m " << msg << std::endl;
}

void error(std::string fileName, unsigned int lineNum, const char* msg){
    std::cerr << "\x1b[1msmplc: " << fileName + ":" << lineNum << " \x1b[91merror:\x1b[0m " << msg << std::endl;
}
void error(const char* msg){
    std::cerr << "\x1b[1msmplc: \x1b[91merror:\x1b[0m " << msg << std::endl;
}

void warning(std::string fileName, unsigned int lineNum, const char* msg){
    std::cerr << "\x1b[1msmplc: " << fileName + ":" << lineNum << " \x1b[93mwarning:\x1b[0m " << msg << std::endl;
}
void warning(const char* msg){
    std::cerr << "\x1b[1msmplc: \x1b[93mwarning:\x1b[0m " << msg << std::endl;
}

void info(std::string fileName, unsigned int lineNum,const char* msg){
    std::cerr << "\x1b[1msmplc: " << fileName + ":" << lineNum << " \x1b[96minfo:\x1b[0m " << msg << std::endl;
}
void info(const char* msg){
    std::cerr << "\x1b[1msmplc: \x1b[96minfo:\x1b[0m " << msg << std::endl;
}

}

#endif