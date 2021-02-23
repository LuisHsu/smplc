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
    struct TypeData{
        enum class Type{
            Var, Array,
        };
        Type type;
        std::vector<size_t> shape;
    };
    struct BlockEntry{
        std::shared_ptr<IR::BasicBlock> root;
        std::unordered_map<std::string, TypeData> variables;
    };
    std::unordered_map<std::string, BlockEntry> blockMap;

    /* Before */
    void beforeParse(Parser::StatSequence&);
    void beforeParse(Parser::Computation&);
    void beforeParse(Parser::WhileStatement&);

    /* After */
    void afterParse(Parser::VarDecl&);
    void afterParse(Parser::StatSequence&);
    void afterParse(Parser::Factor&);
    void afterParse(Parser::Assignment&);
    void afterParse(Parser::Term&);
    void afterParse(Parser::Expression&);
    void afterParse(Parser::IfStatement&);
    void afterParse(Parser::Relation&);
    void afterParse(Parser::WhileStatement&);

private:
    std::string curFunc;
    std::stack<std::unordered_map<std::string, IR::index_t>> varStack;
    std::stack<std::shared_ptr<IR::BasicBlock>> bbStack;
    std::stack<IR::index_t> exprStack;
    template<typename T, typename... O> T& emitInstr(O...);
    std::string getFuncMsg();
};

#endif