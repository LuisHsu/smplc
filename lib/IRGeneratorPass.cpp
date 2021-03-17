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

IRGeneratorPass::IRGeneratorPass(std::unordered_map<std::string, std::shared_ptr<IR::FuncEntry>>& funcMap): funcMap(funcMap){}

template<typename T, typename... O> T& IRGeneratorPass::emitInstr(O... op){
    return std::get<T>(bbStack.top()->instructions.emplace_back(T(op...)));
}

void IRGeneratorPass::afterParse(Parser::VarDecl& target){
    if(target.isSuccess){
        for(Parser::Ident& ident : target.identifiers){
            std::unordered_map<std::string, IR::TypeData>& varMap = curEntry->variables;
            if(varMap.contains(ident.value) || curEntry->paramAddrMap.contains(ident.value)){
                Logger::put(LogLevel::Error, std::string("redifinition of '") + ident.value + "'");
            }else{
                IR::TypeData newVar;
                if(target.typeDecl.declType == Parser::TypeDecl::Type::Variable){
                    newVar.type = IR::TypeData::Type::Var;
                    newVar.shape.push_back(1);
                }else if(target.typeDecl.declType == Parser::TypeDecl::Type::Array){
                    newVar.type = IR::TypeData::Type::Array;
                    IR::address_t arrSize = INT_SIZE;
                    for(Parser::Number& size: target.typeDecl.arraySizes){
                        if(size.value <= 0){
                            Logger::put(LogLevel::Error, std::string("invalid array size of '") + ident.value + "'");
                        }else{
                            newVar.shape.push_back(size.value);
                            arrSize *= size.value;
                        }
                    }
                    newVar.address = stackTop;
                    stackTop += arrSize;
                }
                varMap.insert({ident.value, newVar});
            }
        }
    }
}

void IRGeneratorPass::beforeParse(Parser::StatSequence&){
    if(bbStack.empty()){
        curEntry->root = bbStack.emplace(std::make_shared<IR::BasicBlock>());
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
                std::unordered_map<std::string, IR::TypeData>& varMap = curEntry->variables;
                if(varMap.contains(identName)){
                    IR::TypeData varType = varMap[identName];
                    if(varType.type == IR::TypeData::Type::Var){
                        // Variable
                        std::unordered_map<std::string, IR::index_t>::iterator valIt = varStack.top().find(identName);
                        if(valIt == varStack.top().end()){
                            // Uninitialized
                            Logger::put(LogLevel::Warning, std::string("uninitialized variable '") + identName + "'");
                            IR::Const& instr = emitInstr<IR::Const>((int32_t) 0);
                            exprStack.push(instr.index);
                            varStack.top().emplace(identName, instr.index);
                        }else{
                            exprStack.push(valIt->second);
                        }
                    }else{
                        // Array
                        IR::index_t address = exprStack.top();
                        IR::index_t loadIdx = emitInstr<IR::Load>(address).index;
                        exprStack.pop();
                        exprStack.push(loadIdx);
                    }
                }else if(curEntry->paramAddrMap.contains(identName)){
                    std::unordered_map<std::string, IR::index_t>::iterator valIt = varStack.top().find(identName);
                    if(valIt == varStack.top().end()){
                        IR::Add& address = emitInstr<IR::Add>(IR::Register::fp, emitInstr<IR::Const>((int32_t) curEntry->paramAddrMap[identName]).index);
                        IR::index_t loadIdx = emitInstr<IR::Load>(address.index).index;
                        exprStack.push(loadIdx);
                        varStack.top().emplace(identName, loadIdx);
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
        IR::index_t result = exprStack.top();
        exprStack.pop();
        std::string& identName = target.designator.identifier.value;
        std::unordered_map<std::string, IR::TypeData>& varMap = curEntry->variables;
        if(varMap.find(identName) == varMap.end()){
            Logger::put(LogLevel::Error, std::string("undeclared identifier '") + identName + "'");
        }else{
            if(varMap.at(identName).type == IR::TypeData::Type::Var){
                // Variable
                varStack.top()[identName] = result;
            }else{
                // Array
                IR::index_t address = exprStack.top();
                exprStack.pop();
                emitInstr<IR::Store>(result, address);
            }
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
                    opStack.push(emitInstr<IR::Div>(operand1, operand2).index);
                }else{
                    opStack.push(emitInstr<IR::Mul>(operand1, operand2).index);
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
    stackTop = INT_SIZE * 2;
    funcMap.emplace("_main", std::make_shared<IR::FuncEntry>());
    curEntry = funcMap["_main"];
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
        thenBlock->dominator = previous;
        elseBlock->dominator = previous;

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
        nextBlock->dominator = previous;
        
        // Phi functions
        for(std::pair<std::string, IR::TypeData>&& variable: curEntry->variables){
            if(variable.second.type == IR::TypeData::Type::Var){
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
    whileStack.emplace(bbStack.emplace(std::make_shared<IR::BasicBlock>()));
}

void IRGeneratorPass::afterParse(Parser::WhileStatement& target){
    if(target.isSuccess){
        // Blocks
        std::shared_ptr<IR::BasicBlock> odBlock = bbStack.top();
        std::shared_ptr<IR::BasicBlock> cmpBlock = whileStack.top();
        whileStack.pop();
        while(bbStack.top() != cmpBlock){
            cmpBlock->fallThrough = bbStack.top();
            bbStack.pop();
        }
        odBlock->fallThrough = cmpBlock;
        cmpBlock->fallThrough->dominator = cmpBlock;

        std::shared_ptr<IR::BasicBlock> nextBlock = std::make_shared<IR::BasicBlock>();
        IR::index_t nextInstr = IR::getInstrIndex(nextBlock->instructions.emplace_back(IR::Nop()));
        cmpBlock->branch = nextBlock;
        nextBlock->dominator = cmpBlock;

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
        previous->fallThrough = cmpBlock;
        cmpBlock->dominator = previous;

        // Phi functions
        std::vector<IR::Instrction> phis;
        for(std::pair<std::string, IR::index_t>&& variable: odBlock->variableVal){
            if(curEntry->variables[variable.first].type == IR::TypeData::Type::Var){
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
        }
        cmpBlock->instructions.insert(cmpBlock->instructions.begin(), phis.begin(), phis.end());
        cmpBlock->variableVal = varStack.top();
        bbStack.push(nextBlock);
    }else{
        bbStack.pop();
        whileStack.pop();
    }
}

void IRGeneratorPass::afterParse(Parser::Computation& target){
    if(target.isSuccess){
        emitInstr<IR::End>();
        std::unordered_map<std::string, IR::index_t>& usedVariables = bbStack.top()->variableVal;
        for(auto&& varPair : curEntry->variables){
            if(!usedVariables.contains(varPair.first)){
                Logger::put(LogLevel::Warning, std::string("unused variable '") + varPair.first + "'");
            }
        }
    }else{
        if(!bbStack.empty()){
            bbStack.pop();
        }
    }
}

void IRGeneratorPass::afterParse(Parser::Designator& target){
    if(target.isSuccess){
        std::string& identName = target.identifier.value;
        std::unordered_map<std::string, IR::TypeData>& varMap = curEntry->variables;
        if(varMap.contains(identName)){
            IR::TypeData& varType = varMap.at(identName);         
            if(varType.type == IR::TypeData::Type::Array){
                std::unordered_map<std::string, IR::index_t>::iterator valIt = varStack.top().find(identName);
                IR::index_t address;
                if(valIt == varStack.top().end()){
                    address = emitInstr<IR::Add>(IR::Register::fp, emitInstr<IR::Const>(*(varType.address)).index).index;
                    varStack.top()[identName] = address;
                }else{
                    address = varStack.top().at(identName);
                }
                IR::index_t offset;
                int32_t accSize = INT_SIZE;
                for(size_t i = varType.shape.size(); i > 0; --i){
                    if(i <= target.expressions.size()){
                        IR::Mul& mulInstr = emitInstr<IR::Mul>(exprStack.top(), emitInstr<IR::Const>(accSize).index);
                        exprStack.pop();
                        if(i != target.expressions.size()){
                            offset = emitInstr<IR::Add>(offset, mulInstr.index).index;
                        }else{
                            offset = mulInstr.index;
                        }
                    }
                    accSize *= varType.shape[i - 1];
                }
                exprStack.push(emitInstr<IR::Adda>(address, offset).index);
            }
        }else if(!curEntry->paramAddrMap.contains(identName)){
            Logger::put(LogLevel::Error, std::string("undeclared identifier '") + identName + "'");
        }
    }
}

void IRGeneratorPass::beforeParse(Parser::FuncDecl& target){
    stackTop = INT_SIZE * 2;
    curEntry = std::make_shared<IR::FuncEntry>();
}

void IRGeneratorPass::afterParse(Parser::FuncDecl& target){
    if(target.isSuccess){
        if(target.identifier.value == "InputNum" || target.identifier.value == "OutputNum" || target.identifier.value == "OutputNewLine"){
            Logger::put(LogLevel::Error, target.identifier.value + " is reserved function");
            return;
        }
        funcMap.emplace(target.identifier.value, curEntry);
        std::unordered_map<std::string, IR::index_t>& usedVariables = bbStack.top()->variableVal;
        for(auto&& varPair : curEntry->variables){
            if(!usedVariables.contains(varPair.first)){
                Logger::put(LogLevel::Warning, std::string("unused variable '") + varPair.first + "'");
            }
        }
        for(auto&& varPair : curEntry->paramAddrMap){
            if(!usedVariables.contains(varPair.first)){
                Logger::put(LogLevel::Warning, std::string("unused parameter '") + varPair.first + "'");
            }
        }
        if(!bbStack.empty()){
            bbStack.pop();
        }
    }
    curEntry = funcMap["_main"];
}

void IRGeneratorPass::afterParse(Parser::FormalParam& target){
    if(target.isSuccess){
        for(Parser::Ident identifier : target.identifiers){
            curEntry->paramAddrMap.emplace(identifier.value, stackTop);
            curEntry->paramNames.push_back(identifier.value);
            stackTop += INT_SIZE;
        }
    }
}

void IRGeneratorPass::afterParse(Parser::FuncCall& target){
    if(target.isSuccess){
        std::string& funcName = target.identifier.value;
        if(funcName == "InputNum"){
            exprStack.push(emitInstr<IR::Read>().index);
        }else if(funcName == "OutputNum"){
            emitInstr<IR::Write>(exprStack.top());
            exprStack.pop();
        }else if(funcName == "OutputNewLine"){
            emitInstr<IR::WriteNL>();
        }else if(!funcMap.contains(funcName)){
            Logger::put(LogLevel::Error, std::string("calling undefined function '") + funcName + "'");
        }else{
            std::shared_ptr<IR::FuncEntry>& entry = funcMap.at(funcName);
            if(target.expressions.size() != entry->paramAddrMap.size()){
                Logger::put(LogLevel::Error, std::string("missing parameters while calling function '") + funcName + "'");
                return;
            }
            // Create call link
            IR::FuncCallLink& link = curEntry->callLinks.emplace_back();
            link.funcName = target.identifier.value;
            link.block = bbStack.top();
            // Get destination
            IR::Add& dest = emitInstr<IR::Add>(IR::Register::fp, emitInstr<IR::Const>((int32_t)stackTop).index);
            dest.isImportant = true;
            // Store fp & return address
            emitInstr<IR::Store>(emitInstr<IR::Add>(IR::Register::pc, emitInstr<IR::Const>((int32_t)INT_SIZE).index).index, dest.index);
            emitInstr<IR::Store>(IR::Register::fp,
                emitInstr<IR::Add>(dest.index,
                    emitInstr<IR::Mul>(emitInstr<IR::Const>((int32_t)INT_SIZE).index, emitInstr<IR::Const>((int32_t)2).index).index
                ).index
            );
            // Store parameters
            for(size_t i = 0; i < entry->paramAddrMap.size(); ++i){
                if(exprStack.empty()){
                    Logger::put(LogLevel::Error, std::string("missing expression of parameters for calling function '") + funcName + "'");
                    return;
                }
                IR::Add& address = emitInstr<IR::Add>(dest.index,
                    emitInstr<IR::Const>(entry->paramAddrMap.at(entry->paramNames[i])).index
                );
                emitInstr<IR::Store>(exprStack.top(), address.index);
                link.params.emplace_back(entry->paramNames[i], exprStack.top());
                exprStack.pop();
            }
            // Move frame pointer
            emitInstr<IR::StoreReg>(IR::Register::fp, dest.index);
            // Branch to function
            link.callIndex = emitInstr<IR::Bra>(IR::getInstrIndex(entry->root->instructions.front())).index;
        }
    }
}