/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Source.hpp>

#include <functional>
#include <Exception.hpp>
#include <Logger.hpp>

#include <iostream>

Source::Source(std::istream& stream):
    stream(stream)
{
}

static int isEof(std::istream& stream, std::stack<int>& back){
    return stream.eof() && back.empty();
}

static int getCh(std::istream& stream, std::stack<int>& back){
    if(!back.empty()){
        int ch = back.top();
        back.pop();
        return ch;
    }else{
        return stream.get();
    }
}

static int skipComment(std::istream& stream, std::stack<int>& back){
    if(!isEof(stream, back)){
        int cur = getCh(stream, back);
        if(cur == '\v' || cur == '\r' || cur == '\t' || cur == '\n'){
            if(cur == '\n'){
                Logger::increaseLine();
            }
            cur = ' ';
        }else if(cur == '/' && !isEof(stream, back)){
            int next = getCh(stream, back);
            if(next == '/'){
                // Single line
                while(!isEof(stream, back) && getCh(stream, back) != '\n');
                cur = ' ';
            }else if(next == '*'){
                // Multi line
                int level = 1;
                int prev = 0;
                while(!isEof(stream, back)){
                    next = getCh(stream, back);
                    if(prev == '/' && next == '*'){
                        level += 1;
                    }else if(prev == '*' && next == '/'){
                        level -= 1;
                        if(level == 0){
                            break;
                        }
                    }
                    prev = next;
                }
                if(isEof(stream, back)){
                    if(level != 0){
                        throw Exception("Unclosed multi-line comment");
                    }else{
                        return -1;
                    }
                }
                cur = ' ';
            }else{
                back.push(next);
            }
        }
        return cur;
    }else{
        return -1;
    }
};

int Source::get(){
    if(!stream.eof()){
        int current = skipComment(stream, back);
        if(current == ' '){
            while((current = skipComment(stream, back)) == ' ');
            putback(current);
            current = ' ';
        }
        return current;
    }
    return -1;
}

void Source::putback(int ch){
    if(ch != -1){
        back.push(ch);
    }
}
