#ifndef SMPLC_IRGeneratorPass_DEF
#define SMPLC_IRGeneratorPass_DEF

#include <list>
#include <utility>
#include <vector>
#include <unordered_map>
#include <optional>
#include <string>
#include <stack>
#include <functional>

#include <IR.hpp>
#include <Parser.hpp>

class IRGeneratorPass: public Parser::Pass{
public:
    std::vector<IR::BasicBlock> basicBlocks;

    /* Before */
    void beforeParse(Parser::StatSequence&);
    void beforeParse(Parser::Computation&);

    /* After */
    void afterParse(Parser::VarDecl&);
    void afterParse(Parser::StatSequence&);
    void afterParse(Parser::Factor&);
    void afterParse(Parser::Assignment&);
    void afterParse(Parser::Term&);
    void afterParse(Parser::Expression&);

private:
    struct IdentData{
        enum class Type{
            Var, Array,
        };
        Type type;
        std::vector<size_t> shape;
    };
    std::list<std::pair<std::string, std::unordered_map<std::string, IdentData>>> varList;
    std::stack<std::unordered_map<std::string, IR::index_t>> varStack;
    std::stack<IR::BasicBlock> bbStack;
    std::stack<IR::index_t> exprStack;

    template<typename T> T& emitInstr(bool pushStack = true);
    std::string getFuncMsg();
};

#endif