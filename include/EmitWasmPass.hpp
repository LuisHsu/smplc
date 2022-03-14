#ifndef SMPLC_EmitWasmPass_DEF
#define SMPLC_EmitWasmPass_DEF

#include <Parser.hpp>
#include <Wasm.hpp>
#include <stack>
#include <queue>
#include <utility>
#include <string>
#include <unordered_map>

class EmitWasmPass: public Parser::Pass{
public:
    Wasm::Module module;

protected:
    void beforeParse(Parser::Computation&);
    void afterParse(Parser::Computation&);
    void afterParse(Parser::VarDecl&);
    void beforeParse(Parser::StatSequence&);
    void afterParse(Parser::Factor&);
    void beforeParse(Parser::Term&);
    void afterParse(Parser::Term&);
    void beforeParse(Parser::Expression&);
    void afterParse(Parser::Expression&);
    void beforeParse(Parser::Assignment&);
    void afterParse(Parser::Assignment&);
    void afterParse(Parser::IfStatement&);
    void beforeParse(Parser::Relation&);
    void afterParse(Parser::Relation&);

    struct FuncEntry{
        const uint32_t id;
        std::unordered_map<std::string, uint32_t> identMap;
        Wasm::Func func;
        FuncEntry();
    private:
        static uint32_t serial;
    };
    FuncEntry curFunc;
    std::stack<std::vector<Wasm::Instr>> seqStack;
    std::stack<std::queue<Wasm::Instr>> instrStack;
};

#endif