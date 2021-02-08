#ifndef SMPLC_IRGeneratorPass_DEF
#define SMPLC_IRGeneratorPass_DEF

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

    /* After */
    void afterParse(Parser::VarDecl&);
    void afterParse(Parser::StatSequence&);
    void afterParse(Parser::Factor&);

private:
    struct IdentData{
        enum class Type{
            Var, Array,
        };
        Type type;
        std::vector<size_t> shape;
        std::optional<IR::Operand> value;
    };
    std::unordered_map<std::string, IdentData> identMap;
    std::stack<IR::BasicBlock> bbStack;
};

#endif