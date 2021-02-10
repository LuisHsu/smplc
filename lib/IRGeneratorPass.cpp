/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <IRGeneratorPass.hpp>

#include <string>
#include <Logger.hpp>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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
                IR::Instrction& instr = bbStack.top().instructions.emplace_back(IR::Const{
                    .value = (int32_t)num.value
                });
                exprStack.push(IR::getInstrIndex(instr));
            },
            [&](Parser::Designator& des){
                if(identMap.find(des.identifier.value) == identMap.end()){
                    Logger::put(LogLevel::Error, std::string("undeclared identifier '") + des.identifier.value + "'");
                }else{
                    std::optional<IR::index_t>& identVal = identMap.at(des.identifier.value).value;
                    if(!identVal.has_value()){
                        // Uninitialized
                        Logger::put(LogLevel::Warning, std::string("uninitialized variable '") + des.identifier.value + "'");
                        IR::index_t instr = IR::getInstrIndex(bbStack.top().instructions.emplace_back(IR::Const{
                            .value = 0
                        }));
                        identVal = instr;
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