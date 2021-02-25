/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <IRGeneratorPass.hpp>

#include <stack>
#include <queue>
#include <string>
#include <algorithm>
#include <Logger.hpp>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

IRGeneratorPass::IRGeneratorPass(std::unordered_map<std::string, IR::BlockEntry>& blockMap):blockMap(blockMap){}

std::string IRGeneratorPass::getFuncMsg(){
    return std::string("function ") + curFunc;
}

template<typename T, typename... O> T& IRGeneratorPass::emitInstr(O... op){
    return std::get<T>(bbStack.top()->instructions.emplace_back(T(op...)));
}

void IRGeneratorPass::afterParse(Parser::VarDecl& target){
    if(target.isSuccess){
        for(Parser::Ident& ident : target.identifiers){
            std::unordered_map<std::string, IR::TypeData>& varMap = blockMap.at(curFunc).variables;
            if(varMap.find(ident.value) != varMap.end()){
                Logger::put(LogLevel::Error, std::string("redifinition of '") + ident.value + "' in " + getFuncMsg());
            }else{
                IR::TypeData newVar;
                if(target.typeDecl.declType == Parser::TypeDecl::Type::Variable){
                    newVar.type = IR::TypeData::Type::Var;
                    newVar.shape.push_back(1);
                }else if(target.typeDecl.declType == Parser::TypeDecl::Type::Array){
                    newVar.type = IR::TypeData::Type::Array;
                    for(Parser::Number& size: target.typeDecl.arraySizes){
                        newVar.shape.push_back(size.value);
                    }
                }
                varMap.insert({ident.value, newVar});
            }
        }
    }
}

void IRGeneratorPass::beforeParse(Parser::StatSequence&){
    if(bbStack.empty()){
        blockMap[curFunc].root = bbStack.emplace(std::make_shared<IR::BasicBlock>());
    }else{
        bbStack.emplace(std::make_shared<IR::BasicBlock>());
    }
    if(varStack.empty()){
        varStack.emplace();
    }else{
        varStack.emplace(varStack.top());
    }
}

void IRGeneratorPass::afterParse(Parser::StatSequence& target){
    if(target.isSuccess){
        bbStack.top()->variableVal.swap(varStack.top());
    }else{
        bbStack.pop();
    }
    varStack.pop();
}

void IRGeneratorPass::afterParse(Parser::Factor& target){
    if(target.isSuccess){
        std::visit(overloaded {
            [](auto){},
            [&](Parser::Number& num){

                exprStack.push(emitInstr<IR::Const>((int32_t) num.value).index);
            },
            [&](Parser::Designator& des){
                std::string& identName = des.identifier.value;
                std::unordered_map<std::string, IR::TypeData>& varMap = blockMap.at(curFunc).variables;
                if(varMap.find(identName) == varMap.end()){
                    Logger::put(LogLevel::Error, std::string("undeclared identifier '") + identName + "' in " + getFuncMsg());
                }else{
                    std::unordered_map<std::string, IR::index_t>::iterator valIt = varStack.top().find(identName);
                    if(valIt == varStack.top().end()){
                        // Uninitialized
                        Logger::put(LogLevel::Warning, std::string("uninitialized variable '") + identName + "' in " + getFuncMsg());
                        IR::Const& instr = emitInstr<IR::Const>((int32_t) 0);
                        exprStack.push(instr.index);
                        varStack.top().emplace(identName, instr.index);
                    }else{
                        exprStack.push(valIt->second);
                    }
                }
            },
        }, target.value);
    }
}

void IRGeneratorPass::afterParse(Parser::Assignment& target){
    if(target.isSuccess){
        IR::index_t resultIndex = exprStack.top();
        exprStack.pop();
        std::string& identName = target.designator.identifier.value;
        std::unordered_map<std::string, IR::TypeData>& varMap = blockMap.at(curFunc).variables;
        if(varMap.find(identName) == varMap.end()){
            Logger::put(LogLevel::Error, std::string("undeclared identifier '") + identName + "' in " + getFuncMsg());
        }else{
            varStack.top()[identName] = resultIndex;
        }
    }
}

void IRGeneratorPass::afterParse(Parser::Term& target){
    if(target.isSuccess){
        std::stack<IR::index_t> opStack;
        for(size_t i = 0; i < target.factors.size(); ++i){
            opStack.push(exprStack.top());
            exprStack.pop();
        }
        for(auto&& factor: target.factors){
            if(factor.first != Parser::Term::Type::None){
                IR::index_t operand1, operand2, result;
                operand1 = opStack.top();
                opStack.pop();
                operand2 = opStack.top();
                opStack.pop();
                if(factor.first == Parser::Term::Type::Divide){
                    opStack.push(emitInstr<IR::Mul>(operand1, operand2).index);
                }else{
                    opStack.push(emitInstr<IR::Div>(operand1, operand2).index);
                }
            }
        }
        exprStack.push(opStack.top());
    }
}

void IRGeneratorPass::afterParse(Parser::Expression& target){
    if(target.isSuccess){
        std::stack<IR::index_t> opStack;
        for(size_t i = 0; i < target.terms.size(); ++i){
            opStack.push(exprStack.top());
            exprStack.pop();
        }
        for(auto&& factor: target.terms){
            if(factor.first != Parser::Expression::Type::None){
                IR::index_t operand1, operand2, result;
                operand1 = opStack.top();
                opStack.pop();
                operand2 = opStack.top();
                opStack.pop();
                if(factor.first == Parser::Expression::Type::Plus){
                    opStack.push(emitInstr<IR::Add>(operand1, operand2).index);
                }else{
                    opStack.push(emitInstr<IR::Sub>(operand1, operand2).index);
                }
            }
        }
        exprStack.push(opStack.top());
    }
}

void IRGeneratorPass::beforeParse(Parser::Computation&){
    curFunc = "_main";
    blockMap.emplace(curFunc, IR::BlockEntry());
}

void IRGeneratorPass::afterParse(Parser::IfStatement& target){
    if(target.isSuccess){
        // Blocks
        if(!target.elseStat.has_value()){
            bbStack.emplace(std::make_shared<IR::BasicBlock>());
            bbStack.top()->variableVal = varStack.top();
        }
        
        std::shared_ptr<IR::BasicBlock> elseBlock = bbStack.top();
        bbStack.pop();
        
        std::shared_ptr<IR::BasicBlock> thenBlock = bbStack.top();
        if(thenBlock->instructions.empty()){
            emitInstr<IR::Nop>();
        }
        bbStack.pop();
        
        std::shared_ptr<IR::BasicBlock> previous = bbStack.top();
        previous->variableVal.swap(varStack.top());
        previous->fallThrough = elseBlock;
        previous->branch = thenBlock;

        // Branch instruction
        IR::index_t brachTo = IR::getInstrIndex(thenBlock->instructions.front());
        IR::index_t cmpOperand = exprStack.top();
        exprStack.pop();
        switch (target.relation.opType){
            case Parser::RelOp::Type::Equal :
                emitInstr<IR::Beq>(cmpOperand, brachTo);
                break;
            case Parser::RelOp::Type::NonEqual :
                emitInstr<IR::Bne>(cmpOperand, brachTo);
                break;
            case Parser::RelOp::Type::GreaterEqual :
                emitInstr<IR::Bge>(cmpOperand, brachTo);
                break;
            case Parser::RelOp::Type::GreaterThan :
                emitInstr<IR::Bgt>(cmpOperand, brachTo);
                break;
            case Parser::RelOp::Type::LessEqual :
                emitInstr<IR::Ble>(cmpOperand, brachTo);
                break;
            case Parser::RelOp::Type::LessThan :
                emitInstr<IR::Blt>(cmpOperand, brachTo);
                break;
        }
        bbStack.pop();
        
        // Next block
        std::shared_ptr<IR::BasicBlock>& nextBlock = bbStack.emplace(std::make_shared<IR::BasicBlock>());
        
        elseBlock->instructions.emplace_back(IR::Bra(emitInstr<IR::Nop>().index));
        elseBlock->branch = nextBlock;
        thenBlock->fallThrough = nextBlock;
        
        // Phi functions
        for(std::pair<std::string, IR::TypeData>&& variable: blockMap.at(curFunc).variables){
            if(thenBlock->variableVal.contains(variable.first)){
                if(elseBlock->variableVal.contains(variable.first)){
                    if(thenBlock->variableVal[variable.first] != elseBlock->variableVal[variable.first]){
                        varStack.top()[variable.first] = emitInstr<IR::Phi>(thenBlock->variableVal[variable.first], elseBlock->variableVal[variable.first]).index;
                    }else{
                        varStack.top()[variable.first] = thenBlock->variableVal[variable.first];
                    }
                }else{
                    varStack.top()[variable.first] = emitInstr<IR::Phi>(thenBlock->variableVal[variable.first], emitInstr<IR::Nop>().index).index;
                }
            }else{
                if(elseBlock->variableVal.contains(variable.first)){
                    varStack.top()[variable.first] = emitInstr<IR::Phi>(emitInstr<IR::Nop>().index, elseBlock->variableVal[variable.first]).index;
                }
            }
        }
    }
}

void IRGeneratorPass::afterParse(Parser::Relation& target){
    if(target.isSuccess){
        IR::index_t operand1, operand2;
        operand2 = exprStack.top();
        exprStack.pop();
        operand1 = exprStack.top();
        exprStack.pop();
        exprStack.push(emitInstr<IR::Cmp>(operand1, operand2).index);
    }
}

void IRGeneratorPass::beforeParse(Parser::WhileStatement&){
    if(!varStack.empty()){
        bbStack.top()->variableVal = varStack.top();
    }
    // Block for compare
    bbStack.emplace(std::make_shared<IR::BasicBlock>());
}

void IRGeneratorPass::afterParse(Parser::WhileStatement& target){
    if(target.isSuccess){
        // Blocks
        std::shared_ptr<IR::BasicBlock> doBlock = bbStack.top();
        bbStack.pop();

        std::shared_ptr<IR::BasicBlock> nextBlock = std::make_shared<IR::BasicBlock>();
        IR::index_t nextInstr = IR::getInstrIndex(nextBlock->instructions.emplace_back(IR::Nop()));

        std::shared_ptr<IR::BasicBlock> cmpBlock = bbStack.top();
        cmpBlock->fallThrough = doBlock;
        doBlock->fallThrough = cmpBlock;
        cmpBlock->branch = nextBlock;

        // Branch instruction
        IR::index_t cmpOperand = exprStack.top();
        exprStack.pop();
        switch (target.relation.opType){
            case Parser::RelOp::Type::Equal :
                emitInstr<IR::Beq>(cmpOperand, nextInstr);
                break;
            case Parser::RelOp::Type::NonEqual :
                emitInstr<IR::Bne>(cmpOperand, nextInstr);
                break;
            case Parser::RelOp::Type::GreaterEqual :
                emitInstr<IR::Bge>(cmpOperand, nextInstr);
                break;
            case Parser::RelOp::Type::GreaterThan :
                emitInstr<IR::Bgt>(cmpOperand, nextInstr);
                break;
            case Parser::RelOp::Type::LessEqual :
                emitInstr<IR::Ble>(cmpOperand, nextInstr);
                break;
            case Parser::RelOp::Type::LessThan :
                emitInstr<IR::Blt>(cmpOperand, nextInstr);
                break;
        }
        bbStack.pop();

        std::shared_ptr<IR::BasicBlock> previous = bbStack.top();
        bbStack.pop();
        previous->fallThrough = cmpBlock;

        // Phi functions
        std::vector<IR::Instrction> phis;
        for(std::pair<std::string, IR::index_t>&& variable: doBlock->variableVal){
            if(previous->variableVal.contains(variable.first)){
                if(previous->variableVal.at(variable.first) != variable.second){
                    IR::index_t origin = previous->variableVal.at(variable.first);
                    IR::index_t phiIndex = IR::getInstrIndex(phis.emplace_back(IR::Phi(origin, variable.second)));
                    cmpBlock->relocateMap[origin] = phiIndex;
                    varStack.top()[variable.first] = phiIndex;
                }else{
                    varStack.top()[variable.first] = variable.second;
                }
            }else{
                varStack.top()[variable.first] = variable.second;
            }
        }
        cmpBlock->instructions.insert(cmpBlock->instructions.begin(), phis.begin(), phis.end());
        cmpBlock->variableVal = varStack.top();
        bbStack.push(nextBlock);
    }else{
        bbStack.pop();
    }
}

void IRGeneratorPass::afterParse(Parser::Computation& target){
    if(target.isSuccess){
        std::unordered_map<std::string, IR::index_t>& usedVariables = bbStack.top()->variableVal;
        for(auto&& varPair : blockMap[curFunc].variables){
            if(!usedVariables.contains(varPair.first)){
                Logger::put(LogLevel::Warning, std::string("unused variable '") + varPair.first + "'");
            }
        }
    }else{
        bbStack.pop();
    }
}