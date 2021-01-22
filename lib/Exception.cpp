/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Exception.hpp>

Exception::Exception(std::string message):
    message(message)
{
}
const char* Exception::what(){
    return message.c_str();
}