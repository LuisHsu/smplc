#ifndef SMPLC_IRVisualizerPass_DEF
#define SMPLC_IRVisualizerPass_DEF

#include <IR.hpp>
#include <string>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <list>

class IRVisualizerPass: public IR::Pass{
public:
    IRVisualizerPass(std::string fileName);

private:
    struct Relation{
        std::unordered_map<std::string, std::string> props;
        std::shared_ptr<IR::BasicBlock> from;
        std::shared_ptr<IR::BasicBlock> to;
    };
    std::ofstream fileOut;
    std::unordered_map<std::shared_ptr<IR::BasicBlock>, std::string> bbNames;
    std::list<Relation> relations;
    size_t serialNum;
    std::string curFunc;

    void beforeAll();
    void afterAll();
    void beforeVisit(const std::string&, std::shared_ptr<IR::FuncEntry>&);
    void afterVisit(const std::string&, std::shared_ptr<IR::FuncEntry>&);
    void beforeVisit(std::shared_ptr<IR::BasicBlock>&);
    void afterVisit(std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Nop&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Const&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Neg&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Add&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Sub&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::StoreReg&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Mul&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Div&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Cmp&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Adda&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Load&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Store&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Phi&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::End&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Bra&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Bne&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Beq&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Ble&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Blt&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Bge&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Bgt&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Read&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::Write&, std::shared_ptr<IR::BasicBlock>&);
    void visit(IR::WriteNL&, std::shared_ptr<IR::BasicBlock>&);
};

#endif