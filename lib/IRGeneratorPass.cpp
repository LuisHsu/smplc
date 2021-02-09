/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <IRGeneratorPass.hpp>

#include <Logger.hpp>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void IRGeneratorPass::afterParse(Parser::VarDecl& target){
    if(target.isSuccess){
        for(Parser::Ident& ident : target.identifiers){
            if(identMap.find(ident.identifier) != identMap.end()){
                Logger::put(LogLevel::Error, std::string("redifinition of '") + ident.identifier + "'");
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
                identMap.insert({ident.identifier, newIdent});
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
                bbStack.top().instructions.emplace_back(IR::Const{.value = (int32_t)num.value});
            },
        }, target.value);
    }
}