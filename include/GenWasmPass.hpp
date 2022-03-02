#ifndef SMPLC_GenWasmPass_DEF
#define SMPLC_GenWasmPass_DEF

#include <Parser.hpp>
#include <Wasm.hpp>
#include <stack>

class GenWasmPass: public Parser::Pass{
public:
    Wasm::Module module;
    void output(std::string filePrefix);

    void beforeParse(Parser::Computation&);
    void afterParse(Parser::Computation&);

protected:
    uint32_t funcSerial;
    std::stack<Wasm::Func> funcStack;
};

#endif