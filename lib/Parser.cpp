/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Parser.hpp>

Parser::Interface::Interface(Source& source):
    source(source)
{}

Parser::Letter::Letter(Source& source): Interface(source)
{}
bool Parser::Letter::parse(){
    int letter = source.get();
    return (letter >= 'a') && (letter <= 'z');
}