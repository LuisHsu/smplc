#ifndef SMPLC_IRGeneratorPass_DEF
#define SMPLC_IRGeneratorPass_DEF

#include <vector>
#include <unordered_map>
#include <optional>
#include <string>
#include <stack>
#include <memory>
#include <functional>

#include <IR.hpp>
#include <Parser.hpp>

class IRGeneratorPass: public Parser::Pass{
public:
    IRGeneratorPass(std::unordered_map<std::string, std::shared_ptr<IR::FuncEntry>>& funcMap);

    /* Before */
    void beforeParse(Parser::StatSequence&);
    void beforeParse(Parser::Computation&);
    void beforeParse(Parser::WhileStatement&);
    void beforeParse(Parser::FuncDecl&);

    /* After */
    void afterParse(Parser::VarDecl&);
    void afterParse(Parser::StatSequence&);
    void afterParse(Parser::Factor&);
    void afterParse(Parser::Assignment&);
    void afterParse(Parser::Term&);
    void afterParse(Parser::Expression&);
    void afterParse(Parser::Designator&);
    void afterParse(Parser::IfStatement&);
    void afterParse(Parser::Relation&);
    void afterParse(Parser::WhileStatement&);
    void afterParse(Parser::Computation&);
    void afterParse(Parser::FuncDecl&);
    void afterParse(Parser::FormalParam&);
    void afterParse(Parser::FuncCall&);
    void afterParse(Parser::ReturnStatement&);

private:
    std::unordered_map<std::string, std::shared_ptr<IR::FuncEntry>>& funcMap;
    std::shared_ptr<IR::FuncEntry> curEntry;
    std::stack<std::unordered_map<std::string, IR::index_t>> varStack;
    std::stack<std::shared_ptr<IR::BasicBlock>> bbStack;
    std::stack<std::shared_ptr<IR::BasicBlock>> whileStack;
    std::stack<IR::index_t> exprStack;
    IR::address_t stackTop;
    template<typename T, typename... O> T& emitInstr(O...);
    std::string getFuncMsg();
};

#endif