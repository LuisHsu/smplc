/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Parser.hpp>
#include <Logger.hpp>

template<char ch, char... rem>
static unsigned int matchSequence(Source& source){
    int current = source.get();
    unsigned int res = 0;
    if(current == ch){
        if constexpr (sizeof...(rem) > 0){
            res += 1;
            if((res += matchSequence<rem...>(source)) == (sizeof...(rem) + 1)){
                return res;
            }
        }else{
            return 1;
        }
    }
    source.putback(current);
    return res;
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

static bool errorOnFalse(bool result, std::string errorMsg){
    if(!result){
        Logger::put(LogLevel::Error, std::string("parse error: ") + errorMsg);
    }
    return result;
}

static bool errorOnPartial(unsigned int result, unsigned int expectedLen, std::string errorMsg){
    if(result > 0 && result != expectedLen){
        Logger::put(LogLevel::Error, std::string("parse error: ") + errorMsg);
    }
    return result == expectedLen;
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
        (matchSequence<'=', '='>(source) == 2) ||
        (matchSequence<'!', '='>(source) == 2) ||
        (matchSequence<'>', '='>(source) == 2) ||
        (matchSequence<'<', '='>(source) == 2) ||
        matchOne<'>'>(source) ||
        matchOne<'<'>(source);
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
        errorOnFalse(Expression(source).parse(),
            "expected left expression in relation"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && RelOp(source).parse(),
            "expected relation operator in relation"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && Expression(source).parse(),
            "expected right expression in relation"
        )
    ){
        return true;
    }
    return false;
}

Parser::Assignment::Assignment(Source& source): Interface(source)
{}
bool Parser::Assignment::parse(){
    if(
        errorOnPartial(matchSequence<'l', 'e', 't'>(source), 3,
            "expected 'let' in assignment"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && Designator(source).parse(),
            "expected designator in assignment"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && (matchSequence<'<', '-'>(source) == 2),
            "expected '<-' in assignment"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && Expression(source).parse(),
            "expected expression in assignment"
        )
    ){
        return true;
    }
    return false;
}

Parser::FuncCall::FuncCall(Source& source): Interface(source)
{}
bool Parser::FuncCall::parse(){
    if(
        errorOnPartial(matchSequence<'c', 'a', 'l', 'l'>(source), 4,
            "expected 'call' in function call"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && Ident(source).parse(),
            "expected function name in function call"
        )
    ){
        if(skipWhiteSpaces(source) && matchOne<'('>(source)){
            if(skipWhiteSpaces(source) && Expression(source).parse()){
                while(skipWhiteSpaces(source) && matchOne<','>(source)){
                    if(!errorOnFalse(
                        skipWhiteSpaces(source) && Expression(source).parse(),
                        "expected expression after ',' in function arguments"
                    )){
                        return false;
                    }
                };
            }
            return errorOnFalse(
                skipWhiteSpaces(source) && matchOne<')'>(source),
                "expected ')' after function arguments"
            );
        }
        return true;
    }
    return false;
}

Parser::ReturnStatement::ReturnStatement(Source& source): Interface(source)
{}
bool Parser::ReturnStatement::parse(){
    if(errorOnPartial(matchSequence<'r', 'e', 't', 'u', 'r', 'n'>(source), 6,
        "expected 'return' in return statement"
    )){
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
        errorOnPartial(matchSequence<'i', 'f'>(source), 2,
            "expected 'if' in if statement"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && Relation(source).parse(),
            "expected relation in if statement"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && (matchSequence<'t', 'h', 'e', 'n'>(source) == 4),
            "expected 'then' before if statement body"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && StatSequence(source).parse(),
            "no if statement body"
        )
    ){
        if(
            skipWhiteSpaces(source) && errorOnPartial(matchSequence<'e', 'l', 's', 'e'>(source), 4,
                "expected 'else' in else statement"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && StatSequence(source).parse(),
                "no else statement body"
            )
        ){

        }
        return errorOnFalse(
            skipWhiteSpaces(source) && (matchSequence<'f', 'i'>(source) == 2),
            "expected 'fi' after if statement"
        );
    }
    return false;
}

Parser::WhileStatement::WhileStatement(Source& source): Interface(source)
{}
bool Parser::WhileStatement::parse(){
    if(
        errorOnPartial(matchSequence<'w', 'h', 'i', 'l', 'e'>(source), 5,
            "expected 'while' in while statement"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && Relation(source).parse(),
            "expected relation operator in while statement"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && (matchSequence<'d', 'o'>(source) == 2),
            "expected 'do' before while statement body"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && StatSequence(source).parse(),
            "no while statement body"
        )
        && errorOnFalse(
            skipWhiteSpaces(source)&& (matchSequence<'o', 'd'>(source) == 2),
            "expected 'od' after while statement body"
        )
    ){
        return true;
    }
    return false;
}

Parser::TypeDecl::TypeDecl(Source& source): Interface(source)
{}
bool Parser::TypeDecl::parse(){
    unsigned int varMatch = matchSequence<'v', 'a', 'r'>(source);
    if(varMatch >= 2){
        return errorOnFalse(varMatch == 3,
            "expected 'var' in type declaration"
        );
    }else if(
        errorOnPartial(matchSequence<'a', 'r', 'r', 'a', 'y'>(source), 5,
            "expected 'array' in type declaration"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && matchOne<'['>(source),
            "expected '[' in array type declaration"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && Number(source).parse(),
            "expected number in array type declaration"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && matchOne<']'>(source),
            "expected ']' in array type declaration"
        )
    ){
        while(
            skipWhiteSpaces(source) && matchOne<'['>(source)
            && errorOnFalse(
                skipWhiteSpaces(source) && Number(source).parse(),
                "expected number in array type declaration"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && matchOne<']'>(source),
                "expected ']' in array type declaration"
            )
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
        && errorOnFalse(
            skipWhiteSpaces(source) && Ident(source).parse(),
            "expected identifier in variable declaration"
        )
    ){
        while(
            skipWhiteSpaces(source)
            && matchOne<','>(source)
            && errorOnFalse(
                skipWhiteSpaces(source) && Ident(source).parse(),
                "expected identifier in variable declaration"
            )
        );
        return errorOnFalse(
            skipWhiteSpaces(source) && matchOne<';'>(source),
            "expected ';' after variable declaration"
        );
    }
    return false;
}

Parser::FormalParam::FormalParam(Source& source): Interface(source)
{}
bool Parser::FormalParam::parse(){
    if(errorOnFalse(
        matchOne<'('>(source) && skipWhiteSpaces(source),
        "expected '(' before formal parameters"
    )){
        if(Ident(source).parse()){
            while(
                skipWhiteSpaces(source)
                && matchOne<','>(source)
                && errorOnFalse(
                    skipWhiteSpaces(source) && Ident(source).parse(),
                    "expected identifier after ',' in formal parameters"
                )
            );
        }
        return errorOnFalse(
            skipWhiteSpaces(source) && matchOne<')'>(source),
            "expected ')' after formal parameters"
        );
    }
    return false;
}

Parser::FuncBody::FuncBody(Source& source): Interface(source)
{}
bool Parser::FuncBody::parse(){
    while(VarDecl(source).parse() && skipWhiteSpaces(source));
    if(errorOnFalse(
        matchOne<'{'>(source) && skipWhiteSpaces(source),
        "expected '{' before function body"
    )){
        if(StatSequence(source).parse()){
        }
        return errorOnFalse(
            skipWhiteSpaces(source) && matchOne<'}'>(source),
            "expected '}' after function body"
        );
    }
    return false;
}

Parser::FuncDecl::FuncDecl(Source& source): Interface(source)
{}
bool Parser::FuncDecl::parse(){
    errorOnPartial(matchSequence<'v', 'o', 'i', 'd'>(source), 4,
        "expected 'void' in void function"
    );
    if(skipWhiteSpaces(source) && 
        errorOnPartial(matchSequence<'f','u','n','c','t','i','o','n'>(source), 8,
            "expected 'function' in function declaration"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && Ident(source).parse(),
            "expected identifier in function signature"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && FormalParam(source).parse(),
            "expected formal parameter in function signature"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && matchOne<';'>(source),
            "expected ';' after function signature"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && FuncBody(source).parse(),
            "no function body"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && matchOne<';'>(source),
            "expected ';' after function declaration"
        )
    ){
        return true;
    }
    return false;
}

Parser::Computation::Computation(Source& source): Interface(source)
{}
bool Parser::Computation::parse(){
    if(errorOnFalse(
        skipWhiteSpaces(source) && (matchSequence<'m', 'a', 'i', 'n'>(source) == 4),
        "expected 'main' in the begin of computation"
    )){
        while(skipWhiteSpaces(source) && VarDecl(source).parse());
        while(skipWhiteSpaces(source) && FuncDecl(source).parse());
        if(
            errorOnFalse(
                skipWhiteSpaces(source) && matchOne<'{'>(source), 
                "expected '{' before computation body"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && StatSequence(source).parse(),
                "no statements in computation body"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && matchOne<'}'>(source),
                "expected '}' after computation body"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && matchOne<'.'>(source),
                "expected '.' in the end of computation"
            )
        ){
            return true;
        }
    }
    return false;
}