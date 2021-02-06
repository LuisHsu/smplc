#ifndef SMPLC_IRGeneratorPass_DEF
#define SMPLC_IRGeneratorPass_DEF

#include <vector>
#include <unordered_map>
#include <optional>
#include <string>
#include <IR.hpp>
#include <Parser.hpp>

class IRGeneratorPass: public Parser::Pass{
public:
    std::vector<IR::BasicBlock> basicBlocks;
    void afterParse(Parser::VarDecl&);

private:
    struct IdentData{
        enum class Type{
            Var, Array,
        };
        Type type;
        std::vector<size_t> shape;
        std::optional<IR::index_t> index;
    };
    std::unordered_map<std::string, IdentData> identMap;
};

#endif