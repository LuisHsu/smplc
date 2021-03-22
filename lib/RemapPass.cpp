/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <RemapPass.hpp>

template<typename T>
void unaryRemap(T& instr, std::stack<std::pair<std::shared_ptr<IR::BasicBlock>, std::unordered_map<IR::index_t, IR::index_t>>>& mapStack){
    if(!mapStack.empty() && mapStack.top().second.contains(instr.operand)){
        instr.operand = mapStack.top().second[instr.operand];
    }
}

template<typename T>
void binaryRemap(T& instr, std::stack<std::pair<std::shared_ptr<IR::BasicBlock>, std::unordered_map<IR::index_t, IR::index_t>>>& mapStack){
    if(!mapStack.empty()){
        if(mapStack.top().second.contains(instr.operand1)){
            instr.operand1 = mapStack.top().second[instr.operand1];
        }
        if(mapStack.top().second.contains(instr.operand2)){
            instr.operand2 = mapStack.top().second[instr.operand2];
        }
    }
}

template<typename T>
void branchRemap(T& instr, std::stack<std::pair<std::shared_ptr<IR::BasicBlock>, std::unordered_map<IR::index_t, IR::index_t>>>& mapStack){
    if(!mapStack.empty() && mapStack.top().second.contains(instr.operand1)){
        instr.operand1 = mapStack.top().second[instr.operand1];
    }
}

void RemapPass::beforeVisit(std::shared_ptr<IR::BasicBlock>& block){
    if(block){
        if(!mapStack.empty() && block == mapStack.top().first){
            // After branch
            mapStack.pop();
        }
        if(block->branch){
            if(mapStack.empty()){
                mapStack.emplace(block->branch, block->relocateMap);
            }else{
                std::unordered_map<IR::index_t, IR::index_t> previous = mapStack.top().second;
                mapStack.emplace(block->branch, block->relocateMap);
                mapStack.top().second.merge(previous);
            }
        }
    }
}

void RemapPass::visit(IR::Neg& instr, std::shared_ptr<IR::BasicBlock>&){
    unaryRemap(instr, mapStack);
}

void RemapPass::visit(IR::Add& instr, std::shared_ptr<IR::BasicBlock>&){
    binaryRemap(instr, mapStack);
}

void RemapPass::visit(IR::Sub& instr, std::shared_ptr<IR::BasicBlock>&){
    binaryRemap(instr, mapStack);
}

void RemapPass::visit(IR::Mul& instr, std::shared_ptr<IR::BasicBlock>&){
    binaryRemap(instr, mapStack);
}

void RemapPass::visit(IR::Div& instr, std::shared_ptr<IR::BasicBlock>&){
    binaryRemap(instr, mapStack);
}

void RemapPass::visit(IR::Cmp& instr, std::shared_ptr<IR::BasicBlock>&){
    binaryRemap(instr, mapStack);
}

void RemapPass::visit(IR::Adda& instr, std::shared_ptr<IR::BasicBlock>&){
    binaryRemap(instr, mapStack);
}

void RemapPass::visit(IR::Load& instr, std::shared_ptr<IR::BasicBlock>&){
    unaryRemap(instr, mapStack);
}

void RemapPass::visit(IR::Store& instr, std::shared_ptr<IR::BasicBlock>&){
    binaryRemap(instr, mapStack);
}

void RemapPass::visit(IR::Phi& instr, std::shared_ptr<IR::BasicBlock>&){
    if(!mapStack.empty()){
        if(mapStack.top().second.contains(instr.operand1)){
            IR::index_t index = mapStack.top().second[instr.operand1];
            if(index != instr.index){
                instr.operand1 = index;
            }
        }
        if(mapStack.top().second.contains(instr.operand2)){
            IR::index_t index = mapStack.top().second[instr.operand2];
            if(index != instr.index){
                instr.operand2 = index;
            }
        }
    }
}

void RemapPass::visit(IR::Bra&, std::shared_ptr<IR::BasicBlock>&){
    // Update call link
    if(!mapStack.empty()){
        for(IR::FuncCallLink& link : curEntry->callLinks){
            for(auto& param : link.params){
                if(mapStack.top().second.contains(param.second)){
                    param.second = mapStack.top().second[param.second];
                }
            }
        }
    }
}

void RemapPass::visit(IR::Bne& instr, std::shared_ptr<IR::BasicBlock>&){
    branchRemap(instr, mapStack);
}

void RemapPass::visit(IR::Beq& instr, std::shared_ptr<IR::BasicBlock>&){
    branchRemap(instr, mapStack);
}

void RemapPass::visit(IR::Ble& instr, std::shared_ptr<IR::BasicBlock>&){
    branchRemap(instr, mapStack);
}

void RemapPass::visit(IR::Blt& instr, std::shared_ptr<IR::BasicBlock>&){
    branchRemap(instr, mapStack);
}

void RemapPass::visit(IR::Bge& instr, std::shared_ptr<IR::BasicBlock>&){
    branchRemap(instr, mapStack);
}

void RemapPass::visit(IR::Bgt& instr, std::shared_ptr<IR::BasicBlock>&){
    branchRemap(instr, mapStack);
}

void RemapPass::visit(IR::Write& instr, std::shared_ptr<IR::BasicBlock>&){
    unaryRemap(instr, mapStack);
}

void RemapPass::beforeVisit(const std::string&, std::shared_ptr<IR::FuncEntry>& entry){
    curEntry = entry;
}