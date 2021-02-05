/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <PrintPass.hpp>

#include <iostream>
#include <variant>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void PrintPass::afterParse(Parser::RelOp& target){
    if(target.isSuccess){
        std::cout << "RelOp: ";
        switch (target.opType){
        case Parser::RelOp::Type::Equal :
            std::cout << "==" << std::endl;
            break;
        case Parser::RelOp::Type::NonEqual :
            std::cout << "!=" << std::endl;
            break;
        case Parser::RelOp::Type::GreaterEqual :
            std::cout << ">=" << std::endl;
            break;
        case Parser::RelOp::Type::LessEqual :
            std::cout << "<=" << std::endl;
            break;
        case Parser::RelOp::Type::GreaterThan :
            std::cout << ">" << std::endl;
            break;
        case Parser::RelOp::Type::LessThan :
            std::cout << "<" << std::endl;
            break;
        }
    }
}
void PrintPass::afterParse(Parser::Ident& target){
    if(target.isSuccess){
        std::cout << "Ident: " << target.identifier << std::endl;
    }
}
void PrintPass::afterParse(Parser::Number& target){
    if(target.isSuccess){
        std::cout << "Number: " << target.value << std::endl;
    }
}
void PrintPass::afterParse(Parser::Designator& target){
    if(target.isSuccess){
        std::cout << "Designator[" << target.identifier.identifier <<"]" << std::endl;
    }
}
void PrintPass::afterParse(Parser::Factor& target){
    if(target.isSuccess){
        std::cout << "Factor[";
        std::visit(overloaded {
            [](std::monostate&){},
            [](Parser::Designator&){std::cout << "designator";},
            [](Parser::Number&){std::cout << "number";},
            [](Parser::Expression&){std::cout << "expression";},
            [](Parser::FuncCall&){std::cout << "funcCall";}
        }, target.value);
        std::cout << "]" << std::endl;
    }
}
void PrintPass::afterParse(Parser::Term& target){
    if(target.isSuccess){
        std::cout << "Term: ";
        for(std::pair<Parser::Term::Type, Parser::Factor>& factorPair: target.factors){
            switch (factorPair.first){
            case Parser::Term::Type::Times :
                std::cout << "* ";
                break;
            case Parser::Term::Type::Divide :
                std::cout << "/ ";
                break;
            default:
                std::cout << "null ";
                break;
            }
        }
        std::cout << std::endl;
    }
}
void PrintPass::afterParse(Parser::Expression& target){
    if(target.isSuccess){
        std::cout << "Expression: ";
        for(std::pair<Parser::Expression::Type, Parser::Term>& termPair: target.terms){
            switch (termPair.first){
            case Parser::Expression::Type::Plus :
                std::cout << "+ ";
                break;
            case Parser::Expression::Type::Minus :
                std::cout << "- ";
                break;
            default:
                std::cout << "null ";
                break;
            }
        }
        std::cout << std::endl;
    }
}
void PrintPass::afterParse(Parser::Relation& target){
    if(target.isSuccess){
        std::cout << "Relation: ";
        switch (target.opType){
        case Parser::RelOp::Type::Equal :
            std::cout << "==";
            break;
        case Parser::RelOp::Type::NonEqual :
            std::cout << "!=";
            break;
        case Parser::RelOp::Type::GreaterEqual :
            std::cout << ">=";
            break;
        case Parser::RelOp::Type::LessEqual :
            std::cout << "<=";
            break;
        case Parser::RelOp::Type::GreaterThan :
            std::cout << ">";
            break;
        case Parser::RelOp::Type::LessThan :
            std::cout << "<";
            break;
        }
        std::cout << std::endl;
    }
}
void PrintPass::afterParse(Parser::Assignment& target){
    if(target.isSuccess){
        std::cout << "Assignment" << std::endl;
    }
}

void PrintPass::afterParse(Parser::FuncCall& target){
    if(target.isSuccess){
        std::cout << "FuncCall: " << target.identifier.identifier << " (" << target.expressions.size() << " params)" << std::endl;
    }
}

void PrintPass::afterParse(Parser::IfStatement& target){
    if(target.isSuccess){
        if(target.elseStat.has_value()){
            std::cout << "If (with else)" << std::endl;
        }else{
            std::cout << "If" << std::endl;
        }
    }
}

void PrintPass::afterParse(Parser::WhileStatement& target){
    if(target.isSuccess){
        std::cout << "While" << std::endl;
    }
}

void PrintPass::afterParse(Parser::ReturnStatement& target){
    if(target.isSuccess){
        if(target.expression.isSuccess){
            std::cout << "Return (with expression)" << std::endl;
        }else{
            std::cout << "Return" << std::endl;
        }
    }
}

void PrintPass::afterParse(Parser::Statement& target){
    if(target.isSuccess){
        std::cout << "Statement[";
        std::visit(overloaded {
            [](std::monostate&){},
            [](Parser::Assignment&){std::cout << "assignment";},
            [](Parser::FuncCall&){std::cout << "funcCall";},
            [](Parser::IfStatement&){std::cout << "if";},
            [](Parser::WhileStatement&){std::cout << "while";},
            [](Parser::ReturnStatement&){std::cout << "return";}
        }, target.value);
        std::cout << "]" << std::endl;
    }
}

void PrintPass::afterParse(Parser::StatSequence& target){
    if(target.isSuccess){
        std::cout << "StatSequence (" << target.statements.size() << " statements)" << std::endl;
    }
}

void PrintPass::afterParse(Parser::TypeDecl& target){
    if(target.isSuccess){
        std::cout << "TypeDecl (" << ((target.declType == Parser::TypeDecl::Type::Variable) ? "var" : "array") << ") ";
        for(Parser::Number& num: target.arraySizes){
            std::cout << "[" << num.value << "]";
        }
        std::cout << std::endl;
    }
}

void PrintPass::afterParse(Parser::VarDecl& target){
    if(target.isSuccess){
        std::cout << "VarDecl: ";;
        for(Parser::Ident& ident: target.identifiers){
            std::cout << ident.identifier << " ";
        }
        std::cout << std::endl;
    }
}

void PrintPass::afterParse(Parser::FormalParam& target){
    if(target.isSuccess){
        std::cout << "FormalParam: (";
        for(Parser::Ident& ident: target.identifiers){
            std::cout << ident.identifier << " ";
        }
        std::cout << ")" << std::endl;
    }
}

void PrintPass::afterParse(Parser::FuncBody& target){
    if(target.isSuccess){
        std::cout << "FuncBody ";
        if(target.statSequence.has_value()){
            std::cout << "(with statements): ";
        }else{
            std::cout << "(empty): ";
        }
        for(Parser::VarDecl& varDecl: target.varDecls){
            for(Parser::Ident& ident: varDecl.identifiers){
                std::cout << ident.identifier << " ";
            }
        }
        std::cout << std::endl;
    }
}