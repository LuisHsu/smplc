/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <IR.hpp>
#include <queue>
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

void IR::Pass::beforeVisit(const std::string&, IR::BlockEntry&){}
void IR::Pass::afterVisit(const std::string&, IR::BlockEntry&){}
void IR::Pass::visit(std::shared_ptr<IR::BasicBlock>&){}
void IR::Pass::visit(IR::Nop&){}
void IR::Pass::visit(IR::Const&){}
void IR::Pass::visit(IR::Neg&){}
void IR::Pass::visit(IR::Add&){}
void IR::Pass::visit(IR::Sub&){}
void IR::Pass::visit(IR::Mul&){}
void IR::Pass::visit(IR::Div&){}
void IR::Pass::visit(IR::Cmp&){}
void IR::Pass::visit(IR::Adda&){}
void IR::Pass::visit(IR::Load&){}
void IR::Pass::visit(IR::Store&){}
void IR::Pass::visit(IR::Phi&){}
void IR::Pass::visit(IR::End&){}
void IR::Pass::visit(IR::Bra&){}
void IR::Pass::visit(IR::Bne&){}
void IR::Pass::visit(IR::Beq&){}
void IR::Pass::visit(IR::Ble&){}
void IR::Pass::visit(IR::Blt&){}
void IR::Pass::visit(IR::Bge&){}
void IR::Pass::visit(IR::Bgt&){}
void IR::Pass::visit(IR::Read&){}
void IR::Pass::visit(IR::Write&){}
void IR::Pass::visit(IR::WriteNL&){}

void IR::Pass::traverse(std::unordered_map<std::string, IR::BlockEntry>& blockMap){
    for(std::pair<const std::string, IR::BlockEntry>& blockPair : blockMap){
        beforeVisit(blockPair.first, blockPair.second);
        if(blockPair.second.root){
            std::queue<std::shared_ptr<IR::BasicBlock>> blockQueue;
            std::set<std::shared_ptr<IR::BasicBlock>> traversedSet;
            blockQueue.push(blockPair.second.root);
            traversedSet.emplace(blockPair.second.root);
            while(!blockQueue.empty()){
                std::shared_ptr<IR::BasicBlock> block = blockQueue.front();
                blockQueue.pop();
                visit(block);
                for(IR::Instrction& instr : block->instructions){
                    std::visit([this](auto& arg){
                        visit(arg);
                    }, instr);
                }
                if(block->fallThrough && !traversedSet.contains(block->fallThrough)){
                    blockQueue.push(block->fallThrough);
                    traversedSet.emplace(block->fallThrough);
                }
                if(block->branch && !traversedSet.contains(block->branch)){
                    blockQueue.push(block->branch);
                    traversedSet.emplace(block->branch);
                }
            }
        }
        afterVisit(blockPair.first, blockPair.second);
    }
}