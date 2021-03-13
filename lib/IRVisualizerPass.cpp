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
    for(Relation& relation : relations){
        fileOut << bbNames.at(relation.from) << " -> " << bbNames.at(relation.to);
        if(!relation.props.empty()){
            fileOut << " [";
            for(auto it = relation.props.begin(); it != relation.props.end(); ++it){
                fileOut << it->first << "=" << it->second;
                if(std::next(it) != relation.props.end()){
                    fileOut << ", ";
                }
            }
            fileOut << "];" << std::endl;
        }
    }
    fileOut << "}" << std::endl;
}

void IRVisualizerPass::beforeVisit(const std::string& funcName, std::shared_ptr<IR::BlockEntry>&){
    curFunc = funcName;
}

void IRVisualizerPass::afterVisit(const std::string&, std::shared_ptr<IR::BlockEntry>&){
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
        Relation relation;
        relation.from = target;
        relation.to = target->fallThrough;
        relation.props["label"] = "\"fall-through\"";
        relations.push_back(relation);
    }
    if(target->branch){
        Relation relation;
        relation.from = target;
        relation.to = target->branch;
        relation.props["label"] = "\"branch\"";
        relations.push_back(relation);
    }
    if(target->dominator){
        Relation relation;
        relation.from = target->dominator;
        relation.to = target;
        relation.props["label"] = "\"dom\"";
        relation.props["color"] = "blue";
        relation.props["style"] = "dotted";
        relations.push_back(relation);
    }
}

void IRVisualizerPass::afterVisit(std::shared_ptr<IR::BasicBlock>& target){
    if(target->instructions.empty()){
        fileOut << "\t<tr><td></td></tr>\n";
    }
    fileOut << "</table></td></tr></table>>];\n" << std::endl;
}

static void outputInstr(std::string message, std::ofstream& fileOut, IR::index_t index, std::shared_ptr<IR::BasicBlock>& block){
    fileOut << "\t<tr><td>" << index << ": " << message << "</td></tr>";
    if(index != IR::getInstrIndex(block->instructions.back())){
        fileOut << "<hr/>";
    }
    fileOut << std::endl;
}

static std::string formatOperand(IR::index_t index){
    std::stringstream message;
    switch (index){
    case IR::Register::fp :
        return "(FP)";
    default:
        message << "(" << index << ")";
        return message.str();
    }
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
    outputInstr(
        std::string("neg ") + formatOperand(target.operand),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::Add& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("add ") + formatOperand(target.operand1) + " " + formatOperand(target.operand2),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::Sub& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("sub ") + formatOperand(target.operand1) + " " + formatOperand(target.operand2),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::Mul& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("mul ") + formatOperand(target.operand1) + " " + formatOperand(target.operand2),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::Div& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("div ") + formatOperand(target.operand1) + " " + formatOperand(target.operand2),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::Cmp& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("cmp ") + formatOperand(target.operand1) + " " + formatOperand(target.operand2),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::Adda& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("adda ") + formatOperand(target.operand1) + " " + formatOperand(target.operand2),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::Load& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("load ") + formatOperand(target.operand),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::Store& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("store ") + formatOperand(target.operand1) + " " + formatOperand(target.operand2),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::Phi& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("phi ") + formatOperand(target.operand1) + " " + formatOperand(target.operand2),
        fileOut, target.index, block
    );
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
    message << "bne " << formatOperand(target.operand1) << " :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Beq& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "beq " << formatOperand(target.operand1) << " :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Ble& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "ble " << formatOperand(target.operand1) << " :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Blt& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "blt " << formatOperand(target.operand1) << " :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Bge& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "bge " << formatOperand(target.operand1) << " :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Bgt& target, std::shared_ptr<IR::BasicBlock>& block){
    std::stringstream message;
    message << "bgt " << formatOperand(target.operand1) << " :" << target.operand2;
    outputInstr(message.str(), fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Read& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr("read", fileOut, target.index, block);
}
void IRVisualizerPass::visit(IR::Write& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr(
        std::string("write ") + formatOperand(target.operand),
        fileOut, target.index, block
    );
}
void IRVisualizerPass::visit(IR::WriteNL& target, std::shared_ptr<IR::BasicBlock>& block){
    outputInstr("writeNL", fileOut, target.index, block);
}