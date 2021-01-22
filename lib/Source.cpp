/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Source.hpp>

#include <functional>
#include <Exception.hpp>

Source::Source(std::istream& stream):
    stream(stream)
{
}

static int skipComment(std::istream& stream){
    if(!stream.eof()){
        int cur = stream.get();
        if(cur == '\v' || cur == '\r' || cur == '\t' || cur == '\n'){
            cur = ' ';
        }else if(cur == '/' && !stream.eof()){
            int next = stream.get();
            if(next == '/'){
                // Single line
                while(!stream.eof() && stream.get() != '\n');
                cur = ' ';
            }else if(next == '*'){
                // Multi line
                int level = 1;
                int prev = 0;
                while(!stream.eof()){
                    next = stream.get();
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
                if(stream.eof()){
                    if(level != 0){
                        throw Exception("Unclosed multi-line comment");
                    }else{
                        return -1;
                    }
                }
                cur = ' ';
            }else{
                stream.putback(next);
            }
        }
        return cur;
    }else{
        return -1;
    }
};

int Source::get(){
    if(!stream.eof()){
        int current = skipComment(stream);
        if(current == ' '){
            while((current = skipComment(stream)) == ' ');
            if(current != -1){
                stream.putback(current);
            }
            current = ' ';
        }
        return current;
    }
    return -1;
}

void Source::putback(char ch){
    stream.clear();
    stream.putback(ch);
}
