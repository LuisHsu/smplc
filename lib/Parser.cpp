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
    return matchRange<'a', 'z'>(source, letter);
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
        // TODO: array
        return true;
    }
    return false;
}

Parser::Factor::Factor(Source& source): Interface(source)
{}
bool Parser::Factor::parse(){
    if(
        Designator(source).parse() ||
        Number(source).parse()
        // TODO: Expression
        // TODO: FuncCall
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