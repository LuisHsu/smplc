#ifndef SMPLC_GenWasmPass_DEF
#define SMPLC_GenWasmPass_DEF

#include <Parser.hpp>

class GenWasmPass: public Parser::Pass{
public:

    void output(std::string filePrefix);
};

#endif