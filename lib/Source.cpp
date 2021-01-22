/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Source.hpp>

#include <Exception.hpp>

Source::Source(std::istream& stream):
    stream(stream), lastChar(0)
{
}

int Source::get(){
    if(stream.eof()){
        return -1;
    }
    int current = stream.get();
    if(current == '/'){
        int next = stream.get();
        if(next == '/'){ // Single line comment
            while(!stream.eof() && ((current = stream.get()) != '\n'));
            if(stream.eof()){
                return -1;
            }
        }else if(next == '*'){ // Multi-line comment
            int level = 1;
            current = stream.get();
            int previous = 0;
            while(!stream.eof()){
                previous = current;
                current = stream.get();
                if(current == '*' && previous == '/'){
                    level += 1;
                }else if(current == '/' && previous == '*'){
                    level -= 1;
                    if(level == 0){
                        current = stream.get();
                        break;
                    }
                }
            }
            if(stream.eof()){
                if(level != 0){
                    throw Exception("Unclosed multi-line comment");
                }else{
                    return -1;
                }
            }
        }else{
            stream.unget();
        }
    }
    if(current == ' ' || current == '\v' || current == '\r' || current == '\t' || current == '\n'){ // White space
        current = ' ';
        if(lastChar == ' '){
            while(!stream.eof()){
                current = stream.get();
                if(current != ' ' && current != '\v' && current != '\r' && current != '\t' && current != '\n'){
                    break;
                }
            }
            if(stream.eof()){
                return -1;
            }
        }
    }
    lastChar = current;
    return current;
}

Source& Source::operator>>(int& result){
    result = get();
    return *this;
}