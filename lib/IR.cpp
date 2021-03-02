/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <IR.hpp>
#include <stack>
#include <set>
#include <memory>

IR::InstrBase::InstrBase(){
    static IR::index_t newId = 0;
    index = newId++;
}

const IR::index_t IR::getInstrIndex(const IR::Instrction& instr){
    return std::visit<IR::index_t>([](auto& real){
        return real.index;
    }, instr);
}

void IR::Pass::beforeAll(){}
void IR::Pass::afterAll(){}
void IR::Pass::beforeVisit(const std::string&, IR::BlockEntry&){}
void IR::Pass::afterVisit(const std::string&, IR::BlockEntry&){}
void IR::Pass::beforeVisit(std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::afterVisit(std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Nop&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Const&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Neg&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Add&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Sub&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Mul&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Div&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Cmp&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Adda&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Load&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Store&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Phi&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::End&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Bra&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Bne&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Beq&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Ble&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Blt&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Bge&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Bgt&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Read&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Write&, std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::WriteNL&, std::shared_ptr<IR::BasicBlock>&){}

void IR::Pass::traverse(std::unordered_map<std::string, IR::BlockEntry>& blockMap){
    beforeAll();
    for(std::pair<const std::string, IR::BlockEntry>& blockPair : blockMap){
        beforeVisit(blockPair.first, blockPair.second);
        if(blockPair.second.root){
            std::stack<std::shared_ptr<IR::BasicBlock>> blockStack;
            std::set<std::shared_ptr<IR::BasicBlock>> traversedSet;
            blockStack.push(blockPair.second.root);
            traversedSet.emplace(blockPair.second.root);
            while(!blockStack.empty()){
                std::shared_ptr<IR::BasicBlock> block = blockStack.top();
                blockStack.pop();
                beforeVisit(block);
                for(IR::Instrction& instr : block->instructions){
                    std::visit([this, &block](auto& arg){
                        visit(arg, block);
                    }, instr);
                }
                afterVisit(block);
                if(block->branch && !traversedSet.contains(block->branch)){
                    blockStack.push(block->branch);
                    traversedSet.emplace(block->branch);
                }
                if(block->fallThrough && !traversedSet.contains(block->fallThrough)){
                    blockStack.push(block->fallThrough);
                    traversedSet.emplace(block->fallThrough);
                }
            }
        }
        afterVisit(blockPair.first, blockPair.second);
    }
    afterAll();
}