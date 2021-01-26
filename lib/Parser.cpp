/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Parser.hpp>

template<char ch, char... rem>
static bool matchAllOf(Source& source){
    int current = source.get();
    if(current == ch){
        if constexpr (sizeof...(rem) > 0){
            if(matchAllOf<rem...>(source)){
                return true;
            }
        }else{
            return true;
        }
    }
    source.putback(current);
    return false;
}

template<char lower, char upper>
static bool matchRange(Source& source, int& ret){
    ret = source.get();
    if((ret >= lower) && (ret <= upper)){
        return true;
    }else{
        source.putback(ret);
        ret = 0;
        return false;
    }
}

template<char target>
static bool matchOne(Source& source){
    int current = source.get();
    if(current == target){
        return true;
    }else{
        source.putback(current);
        return false;
    }
}

static bool skipWhiteSpaces(Source& source){
    int current;
    while((current = source.get()) == ' ');
    source.putback(current);
    return true;
}

Parser::Interface::Interface(Source& source):
    source(source)
{}

Parser::Letter::Letter(Source& source): Interface(source)
{}
bool Parser::Letter::parse(){
    int letter;
    return matchRange<'a', 'z'>(source, letter) || matchRange<'A', 'Z'>(source, letter);
}

Parser::Digit::Digit(Source& source): Interface(source)
{}
bool Parser::Digit::parse(){
    int digit;
    return matchRange<'0', '9'>(source, digit);
}

Parser::RelOp::RelOp(Source& source): Interface(source)
{}
bool Parser::RelOp::parse(){
    return
        matchAllOf<'=', '='>(source) ||
        matchAllOf<'!', '='>(source) ||
        matchAllOf<'>', '='>(source) ||
        matchAllOf<'<', '='>(source) ||
        matchOne<'<'>(source) ||
        matchOne<'>'>(source);
}

Parser::Ident::Ident(Source& source): Interface(source)
{}
bool Parser::Ident::parse(){
    if(Letter(source).parse()){
        while(Letter(source).parse() || Digit(source).parse());
        return true;
    }
    return false;
}

Parser::Number::Number(Source& source): Interface(source)
{}
bool Parser::Number::parse(){
    if(Digit(source).parse()){
        while(Digit(source).parse());
        return true;
    }
    return false;
}

Parser::Designator::Designator(Source& source): Interface(source)
{}
bool Parser::Designator::parse(){
    if(Ident(source).parse()){
        while(skipWhiteSpaces(source) && matchOne<'['>(source)){
            if(!(
                skipWhiteSpaces(source)
                && Expression(source).parse()
                && skipWhiteSpaces(source)
                && matchOne<']'>(source)
            )){
                return false;
            }
        }
        return true;
    }
    return false;
}

Parser::Factor::Factor(Source& source): Interface(source)
{}
bool Parser::Factor::parse(){
    if(
        FuncCall(source).parse() ||
        (
            matchOne<'('>(source)
            && skipWhiteSpaces(source)
            && Expression(source).parse()
            && skipWhiteSpaces(source)
            && matchOne<')'>(source)
        ) || 
        Designator(source).parse() ||
        Number(source).parse()
    ){
        return true;
    }
    return false;
}

Parser::Term::Term(Source& source): Interface(source)
{}
bool Parser::Term::parse(){
    if(Factor(source).parse()){
        while(
            skipWhiteSpaces(source)
            && (matchOne<'*'>(source) || matchOne<'/'>(source))
            && skipWhiteSpaces(source)
            && Factor(source).parse()
        );
        return true;
    }
    return false;
}

Parser::Expression::Expression(Source& source): Interface(source)
{}
bool Parser::Expression::parse(){
    if(Term(source).parse()){
        while(
            skipWhiteSpaces(source)
            && (matchOne<'+'>(source) || matchOne<'-'>(source))
            && skipWhiteSpaces(source)
            && Term(source).parse()
        );
        return true;
    }
    return false;
}

Parser::Relation::Relation(Source& source): Interface(source)
{}
bool Parser::Relation::parse(){
    if(
        Expression(source).parse()
        && skipWhiteSpaces(source)
        && RelOp(source).parse()
        && skipWhiteSpaces(source)
        && Expression(source).parse()
    ){
        return true;
    }
    return false;
}

Parser::Assignment::Assignment(Source& source): Interface(source)
{}
bool Parser::Assignment::parse(){
    if(
        matchAllOf<'l', 'e', 't'>(source)
        && skipWhiteSpaces(source)
        && Designator(source).parse()
        && skipWhiteSpaces(source)
        && matchAllOf<'<', '-'>(source)
        && skipWhiteSpaces(source)
        && Expression(source).parse()
    ){
        return true;
    }
    return false;
}

Parser::FuncCall::FuncCall(Source& source): Interface(source)
{}
bool Parser::FuncCall::parse(){
    if(
        matchAllOf<'c', 'a', 'l', 'l'>(source)
        && skipWhiteSpaces(source)
        && Ident(source).parse()
    ){
        if(skipWhiteSpaces(source) && matchOne<'('>(source)){
            if(skipWhiteSpaces(source) && Expression(source).parse()){
                while(skipWhiteSpaces(source) && matchOne<','>(source)){
                    if(skipWhiteSpaces(source) && !Expression(source).parse()){
                        return false;
                    }
                }
            }
            if(skipWhiteSpaces(source) && !matchOne<')'>(source)){
                return false;
            }
        }
        return true;
    }
    return false;
}

Parser::ReturnStatement::ReturnStatement(Source& source): Interface(source)
{}
bool Parser::ReturnStatement::parse(){
    if(matchAllOf<'r', 'e', 't', 'u', 'r', 'n'>(source)){
        if(skipWhiteSpaces(source) && Expression(source).parse()){
            return true;
        }
        return true;
    }
    return false;
}

Parser::Statement::Statement(Source& source): Interface(source)
{}
bool Parser::Statement::parse(){
    if(
        Assignment(source).parse()
        || FuncCall(source).parse()
        || ReturnStatement(source).parse()
        || IfStatement(source).parse()
        || WhileStatement(source).parse()
    ){
        return true;
    }
    return false;
}

Parser::StatSequence::StatSequence(Source& source): Interface(source)
{}
bool Parser::StatSequence::parse(){
    if(Statement(source).parse()){
        while(skipWhiteSpaces(source) && matchOne<';'>(source)){
            if(skipWhiteSpaces(source) && Statement(source).parse()){
                
            }
        }
        return true;
    }
    return false;
}

Parser::IfStatement::IfStatement(Source& source): Interface(source)
{}
bool Parser::IfStatement::parse(){
    if(
        matchAllOf<'i', 'f'>(source)
        && skipWhiteSpaces(source)
        && Relation(source).parse()
        && skipWhiteSpaces(source)
        && matchAllOf<'t', 'h', 'e', 'n'>(source)
        && skipWhiteSpaces(source)
        && StatSequence(source).parse()
    ){
        if(
            skipWhiteSpaces(source)
            && matchAllOf<'e', 'l', 's', 'e'>(source)
            && skipWhiteSpaces(source)
            && StatSequence(source).parse()
        ){

        }
        return skipWhiteSpaces(source) && matchAllOf<'f', 'i'>(source);
    }
    return false;
}

Parser::WhileStatement::WhileStatement(Source& source): Interface(source)
{}
bool Parser::WhileStatement::parse(){
    if(
        matchAllOf<'w', 'h', 'i', 'l', 'e'>(source)
        && skipWhiteSpaces(source)
        && Relation(source).parse()
        && skipWhiteSpaces(source)
        && matchAllOf<'d', 'o'>(source)
        && skipWhiteSpaces(source)
        && StatSequence(source).parse()
        && skipWhiteSpaces(source)
        && matchAllOf<'o', 'd'>(source)
    ){
        return true;
    }
    return false;
}

Parser::TypeDecl::TypeDecl(Source& source): Interface(source)
{}
bool Parser::TypeDecl::parse(){
    if(matchAllOf<'v', 'a', 'r'>(source)){
        return true;
    }else if(
        matchAllOf<'a', 'r', 'r', 'a', 'y'>(source)
        && skipWhiteSpaces(source)
        && matchOne<'['>(source)
        && skipWhiteSpaces(source)
        && Number(source).parse()
        && skipWhiteSpaces(source)
        && matchOne<']'>(source)
    ){
        while(
            skipWhiteSpaces(source)
            && matchOne<'['>(source)
            && skipWhiteSpaces(source)
            && Number(source).parse()
            && skipWhiteSpaces(source)
            && matchOne<']'>(source)
        );
        return true;
    }
    return false;
}

Parser::VarDecl::VarDecl(Source& source): Interface(source)
{}
bool Parser::VarDecl::parse(){
    if(
        TypeDecl(source).parse()
        && skipWhiteSpaces(source)
        && Ident(source).parse()
    ){
        while(
            skipWhiteSpaces(source)
            && matchOne<','>(source)
            && skipWhiteSpaces(source)
            && Ident(source).parse()
        );
        return skipWhiteSpaces(source) && matchOne<';'>(source);
    }
    return false;
}

Parser::FormalParam::FormalParam(Source& source): Interface(source)
{}
bool Parser::FormalParam::parse(){
    if(
        matchOne<'('>(source)
        && skipWhiteSpaces(source)
    ){
        if(Ident(source).parse()){
            while(
                skipWhiteSpaces(source)
                && matchOne<','>(source)
                && skipWhiteSpaces(source)
                && Ident(source).parse()
            );
        }
        return skipWhiteSpaces(source) && matchOne<')'>(source);
    }
    return false;
}

Parser::FuncBody::FuncBody(Source& source): Interface(source)
{}
bool Parser::FuncBody::parse(){
    while(VarDecl(source).parse() && skipWhiteSpaces(source));
    if(
        matchOne<'{'>(source)
        && skipWhiteSpaces(source)
    ){
        if(StatSequence(source).parse()){
        }
        return skipWhiteSpaces(source) && matchOne<'}'>(source);
    }
    return false;
}