#ifndef SMPLC_RemapPass_DEF
#define SMPLC_RemapPass_DEF

#include <IR.hpp>
#include <string>
#include <stack>
#include <memory>
#include <unordered_map>
#include <utility>

class RemapPass: public IR::Pass{
private:
    std::stack<std::pair<std::shared_ptr<IR::BasicBlock>, std::unordered_map<IR::index_t, IR::index_t>>> mapStack;

    void beforeVisit(std::shared_ptr<IR::BasicBlock>&);

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
    void visit(IR::Bne&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Beq&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Ble&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Blt&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Bge&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Bgt&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Write&, std::shared_ptr<IR::BasicBlock>&);
};

#endif