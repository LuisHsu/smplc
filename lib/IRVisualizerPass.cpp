/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <IRVisualizerPass.hpp>

#include <string>
#include <sstream>

IRVisualizerPass::IRVisualizerPass(std::string fileName) :
    fileOut(fileName, std::ios::out), serialNum(0)
{}

void IRVisualizerPass::beforeAll(){
    fileOut << "digraph {" << std::endl;
    fileOut << "node [shape=plaintext]" << std::endl;
}

void IRVisualizerPass::afterAll(){
    for(std::tuple<std::string, std::shared_ptr<IR::BasicBlock>, std::shared_ptr<IR::BasicBlock>>& relation : relations){
        std::string from = bbNames[std::get<1>(relation)];
        std::string to = bbNames[std::get<2>(relation)];
        std::string label = std::get<0>(relation);
        fileOut << from << " -> " << to << " [label=\"" << label << "\"];" << std::endl;
    }
    fileOut << "\n}" << std::endl;
}

void IRVisualizerPass::beforeVisit(const std::string& funcName, IR::BlockEntry&){
    curFunc = funcName;
}

void IRVisualizerPass::afterVisit(const std::string&, IR::BlockEntry&){
    serialNum = 0;
}

void IRVisualizerPass::beforeVisit(std::shared_ptr<IR::BasicBlock>& target){
    serialNum += 1;
    std::stringstream bbName;
    bbName << curFunc << "_" << serialNum;
    fileOut << bbName.str()
        << "[label=<<table cellspacing=\"0\" cellborder=\"0\"><tr><td>"
        << bbName.str()
        << " </td><vr/><td><table cellspacing=\"0\" cellborder=\"0\" border=\"0\">"
        << std::endl;
    bbNames[target] = bbName.str();
    if(target->fallThrough){
        relations.emplace_back("fall-through", target, target->fallThrough);
    }
    if(target->branch){
        relations.emplace_back("branch", target, target->branch);
    }
}

void IRVisualizerPass::afterVisit(std::shared_ptr<IR::BasicBlock>& target){
    fileOut << "</table></td></tr></table>>];\n" << std::endl;
}

static void outputInstr(std::string message, std::ofstream& fileOut, IR::index_t index, std::shared_ptr<IR::BasicBlock>& block){
    fileOut << "\t<tr><td>" << index << ": " << message << "</td></tr>";
    if(index != IR::getInstrIndex(block->instructions.back())){
        fileOut << "<hr/>";
    }
    fileOut << std::endl;
}

void IRVisualizerPass::visit(IR::Nop& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr("nop", fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Const& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "const #" << target.value;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Neg& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "neg (" << target.operand << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Add& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "add (" << target.operand1 << ") (" << target.operand2 << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Sub& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "sub (" << target.operand1 << ") (" << target.operand2 << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Mul& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "mul (" << target.operand1 << ") (" << target.operand2 << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Div& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "div (" << target.operand1 << ") (" << target.operand2 << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Cmp& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "cmp (" << target.operand1 << ") (" << target.operand2 << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Adda& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "adda (" << target.operand1 << ") (" << target.operand2 << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Load& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "load (" << target.operand << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Store& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "store (" << target.operand1 << ") (" << target.operand2 << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Phi& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "phi (" << target.operand1 << ") (" << target.operand2 << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::End& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr("end", fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Bra& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "bra :" << target.operand;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Bne& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "bne (" << target.operand1 << ") :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Beq& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "beq (" << target.operand1 << ") :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Ble& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "ble (" << target.operand1 << ") :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Blt& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "blt (" << target.operand1 << ") :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Bge& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "bge (" << target.operand1 << ") :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Bgt& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "bgt (" << target.operand1 << ") :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Read& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr("read", fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Write& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "write (" << target.operand << ")";
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::WriteNL& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr("writeNL", fileOut, target.index, block);
}