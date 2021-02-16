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


template<typename T> T& IRGeneratorPass::emitInstr(bool pushStack){
    IR::Instrction& instr = bbStack.top().instructions.emplace_back(T());
    if(pushStack){
        exprStack.push(IR::getInstrIndex(instr));
    }
    return std::get<T>(instr);
}

void IRGeneratorPass::afterParse(Parser::VarDecl& target){
    if(target.isSuccess){
        for(Parser::Ident& ident : target.identifiers){
            if(identMap.find(ident.value) != identMap.end()){
                Logger::put(LogLevel::Error, std::string("redifinition of '") + ident.value + "'");
            }else{
                IdentData newIdent;
                if(target.typeDecl.declType == Parser::TypeDecl::Type::Variable){
                    newIdent.type = IdentData::Type::Var;
                    newIdent.shape.push_back(1);
                }else if(target.typeDecl.declType == Parser::TypeDecl::Type::Array){
                    newIdent.type = IdentData::Type::Array;
                    for(Parser::Number& size: target.typeDecl.arraySizes){
                        newIdent.shape.push_back(size.value);
                    }
                }
                identMap.insert({ident.value, newIdent});
            }
        }
    }
}

void IRGeneratorPass::beforeParse(Parser::StatSequence&){
    bbStack.emplace();
}

void IRGeneratorPass::afterParse(Parser::StatSequence& target){
    if(!target.isSuccess){
        bbStack.pop();
    }
}

void IRGeneratorPass::afterParse(Parser::Factor& target){
    if(target.isSuccess){
        std::visit(overloaded {
            [](auto){},
            [&](Parser::Number& num){
                IR::Const& instr = emitInstr<IR::Const>();
                instr.value = (int32_t)num.value;
            },
            [&](Parser::Designator& des){
                if(identMap.find(des.identifier.value) == identMap.end()){
                    Logger::put(LogLevel::Error, std::string("undeclared identifier '") + des.identifier.value + "'");
                }else{
                    std::optional<IR::index_t>& identVal = identMap.at(des.identifier.value).value;
                    if(!identVal.has_value()){
                        // Uninitialized
                        Logger::put(LogLevel::Warning, std::string("uninitialized variable '") + des.identifier.value + "'");
                        IR::Const& instr = emitInstr<IR::Const>();
                        instr.value = 0;
                        identVal = instr.index;
                    }
                    exprStack.push(*identVal);
                }
            },
        }, target.value);
    }
}

void IRGeneratorPass::afterParse(Parser::Assignment& target){
    if(target.isSuccess){
        IR::index_t resultIndex = exprStack.top();
        exprStack.pop();
        if(identMap.find(target.designator.identifier.value) == identMap.end()){
            Logger::put(LogLevel::Error, std::string("undeclared identifier '") + target.designator.identifier.value + "'");
        }else{
            identMap.at(target.designator.identifier.value).value = resultIndex;
        }
    }
}

void IRGeneratorPass::afterParse(Parser::Term& target){
    if(target.isSuccess){
        std::stack<IR::index_t> opStack;
        std::queue<std::variant<
            std::reference_wrapper<IR::Mul>,
            std::reference_wrapper<IR::Div>
        >> instrQueue;
        for(std::pair<Parser::Term::Type, Parser::Factor>& factor: target.factors){
            opStack.push(exprStack.top());
            exprStack.pop();
            if(factor.first == Parser::Term::Type::Times){
                instrQueue.emplace(emitInstr<IR::Mul>(false));
            }else if(factor.first == Parser::Term::Type::Divide){
                instrQueue.emplace(emitInstr<IR::Div>(false));
            }
        }
        while(!instrQueue.empty()){
            std::visit(overloaded{
                [&](std::reference_wrapper<IR::Mul>& instr){
                    instr.get().operand1 = opStack.top();
                    opStack.pop();
                    instr.get().operand2 = opStack.top();
                    opStack.pop();
                    opStack.push(instr.get().index);
                },
                [&](std::reference_wrapper<IR::Div>& instr){
                    instr.get().operand1 = opStack.top();
                    opStack.pop();
                    instr.get().operand2 = opStack.top();
                    opStack.pop();
                    opStack.push(instr.get().index);
                }
            }, instrQueue.front());
            instrQueue.pop();
        }
        exprStack.push(opStack.top());
    }
}

void IRGeneratorPass::afterParse(Parser::Expression& target){
    if(target.isSuccess){
        std::stack<IR::index_t> opStack;
        std::queue<std::variant<
            std::reference_wrapper<IR::Add>,
            std::reference_wrapper<IR::Sub>
        >> instrQueue;
        for(std::pair<Parser::Expression::Type, Parser::Term>& factor: target.terms){
            opStack.push(exprStack.top());
            exprStack.pop();
            if(factor.first == Parser::Expression::Type::Plus){
                instrQueue.emplace(emitInstr<IR::Add>(false));
            }else if(factor.first == Parser::Expression::Type::Minus){
                instrQueue.emplace(emitInstr<IR::Sub>(false));
            }
        }
        while(!instrQueue.empty()){
            std::visit(overloaded{
                [&](std::reference_wrapper<IR::Add>& instr){
                    instr.get().operand1 = opStack.top();
                    opStack.pop();
                    instr.get().operand2 = opStack.top();
                    opStack.pop();
                    opStack.push(instr.get().index);
                },
                [&](std::reference_wrapper<IR::Sub>& instr){
                    instr.get().operand1 = opStack.top();
                    opStack.pop();
                    instr.get().operand2 = opStack.top();
                    opStack.pop();
                    opStack.push(instr.get().index);
                }
            }, instrQueue.front());
            instrQueue.pop();
        }
        exprStack.push(opStack.top());
    }
}