/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <CSEPass.hpp>

#include <sstream>
#include <algorithm>

void CSEPass::beforeVisit(std::shared_ptr<IR::BasicBlock>& target){
    if(contextStack.empty()){
        contextStack.emplace();
    }
    if(target->dominator && target->dominator->branch == target){
        contextStack.pop();
    }
}
void CSEPass::afterVisit(std::shared_ptr<IR::BasicBlock>& target){
    std::erase_if(target->instructions, [this](IR::Instrction& instr) -> bool {
        return removedSet.contains(IR::getInstrIndex(instr));
    });
    if(target->branch && target->fallThrough){
        contextStack.emplace(contextStack.top());
    }
}

void CSEPass::visit(IR::Const& instr, std::shared_ptr<IR::BasicBlock>& block){
    Context& context = contextStack.top();
    if(context.constMap.contains(instr.value)){
        context.forward[instr.index] = context.constMap.at(instr.value);
        removedSet.insert(instr.index);
    }else{
        context.constMap[instr.value] = instr.index;
    }
}

void CSEPass::visit(IR::Neg& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand)){
        instr.operand = context.forward[instr.operand];
    }
}
void CSEPass::visit(IR::Add& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
    if(context.forward.contains(instr.operand2)){
        instr.operand2 = context.forward[instr.operand2];
    }
}
void CSEPass::visit(IR::Sub& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
    if(context.forward.contains(instr.operand2)){
        instr.operand2 = context.forward[instr.operand2];
    }
}
void CSEPass::visit(IR::Mul& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
    if(context.forward.contains(instr.operand2)){
        instr.operand2 = context.forward[instr.operand2];
    }
    std::pair<IR::index_t, IR::index_t> operandPair = std::make_pair(instr.operand1, instr.operand2);
    if(context.mulMap.contains(operandPair)){
        context.forward[instr.index] = context.mulMap.at(operandPair);
        removedSet.insert(instr.index);
    }else{
        context.mulMap[operandPair] = instr.index;
    }
}
void CSEPass::visit(IR::Div& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
    if(context.forward.contains(instr.operand2)){
        instr.operand2 = context.forward[instr.operand2];
    }
}
void CSEPass::visit(IR::Cmp& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
    if(context.forward.contains(instr.operand2)){
        instr.operand2 = context.forward[instr.operand2];
    }
}
void CSEPass::visit(IR::Adda& instr, std::shared_ptr<IR::BasicBlock>&){
    // TODO:
}
void CSEPass::visit(IR::Load& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand)){
        instr.operand = context.forward[instr.operand];
    }
}
void CSEPass::visit(IR::Store& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
    if(context.forward.contains(instr.operand2)){
        instr.operand2 = context.forward[instr.operand2];
    }
}
void CSEPass::visit(IR::Phi& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
    if(context.forward.contains(instr.operand2)){
        instr.operand2 = context.forward[instr.operand2];
    }
}
void CSEPass::visit(IR::Bne& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
}
void CSEPass::visit(IR::Beq& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
}
void CSEPass::visit(IR::Ble& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
}
void CSEPass::visit(IR::Blt& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
}
void CSEPass::visit(IR::Bge& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
}
void CSEPass::visit(IR::Bgt& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand1)){
        instr.operand1 = context.forward[instr.operand1];
    }
}
void CSEPass::visit(IR::Write& instr, std::shared_ptr<IR::BasicBlock>&){
    Context& context = contextStack.top();
    if(context.forward.contains(instr.operand)){
        instr.operand = context.forward[instr.operand];
    }
}
