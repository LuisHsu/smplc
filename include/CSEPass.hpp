#ifndef SMPLC_CSEPass_DEF
#define SMPLC_CSEPass_DEF

#include <IR.hpp>
#include <string>
#include <stack>
#include <memory>
#include <unordered_map>
#include <map>
#include <utility>
#include <variant>
#include <set>

class CSEPass: public IR::Pass{
private:
    struct Context{
        std::unordered_map<IR::index_t, IR::index_t> forward;
        std::unordered_map<int32_t, IR::index_t> constMap;
        std::unordered_map<IR::index_t, IR::index_t> negMap;
        std::map<std::pair<IR::index_t, IR::index_t>, IR::index_t> addMap, subMap, mulMap, divMap, cmpMap, addaMap;
    };

    std::stack<Context> contextStack;
    std::set<IR::index_t> removedSet;
    
    void beforeVisit(std::shared_ptr<IR::BasicBlock>&);
    void afterVisit(std::shared_ptr<IR::BasicBlock>&);
    
    void visit(IR::Const&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Neg&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Add&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Sub&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Mul&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Div&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Cmp&, std::shared_ptr<IR::BasicBlock>&);

    void visit(IR::Adda&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Load&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Store&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Phi&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Write&, std::shared_ptr<IR::BasicBlock>&);

    void visit(IR::Bne&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Beq&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Ble&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Blt&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Bge&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Bgt&, std::shared_ptr<IR::BasicBlock>&);
};

#endif