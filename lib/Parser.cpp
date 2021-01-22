/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Parser.hpp>

template<char ch, char... rem>
bool matchAllOf(Source& source){
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
bool matchRange(Source& source, int& ret){
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
bool matchOne(Source& source){
    int current = source.get();
    if(current == target){
        return true;
    }else{
        source.putback(current);
        return false;
    }
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