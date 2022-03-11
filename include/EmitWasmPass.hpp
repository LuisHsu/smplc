#ifndef SMPLC_EmitWasmPass_DEF
#define SMPLC_EmitWasmPass_DEF

#include <Parser.hpp>
#include <Wasm.hpp>
#include <stack>
#include <string>
#include <unordered_map>

class EmitWasmPass: public Parser::Pass{
public:
    Wasm::Module module;

protected:
    void beforeParse(Parser::Computation&);
    void afterParse(Parser::Computation&);
    void afterParse(Parser::VarDecl&);
    void afterParse(Parser::StatSequence&);
    void afterParse(Parser::Factor&);

    struct FuncEntry{
        const uint32_t id;
        std::unordered_map<std::string, uint32_t> identMap;
        Wasm::Func func;
        FuncEntry();
    private:
        static uint32_t serial;
    };
    std::stack<FuncEntry> funcStack;
};

#endif