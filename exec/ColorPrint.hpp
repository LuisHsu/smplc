#ifndef SMPLC_ColorMessage_DEF
#define SMPLC_ColorMessage_DEF

#include <iostream>

namespace ColorPrint{
    
void fatal(const char* msg){
    std::cerr << "\x1b[1msmplc: \x1b[91mfatal error:\x1b[0m " << msg << std::endl;
}

}

#endif