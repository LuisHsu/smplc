/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <IR.hpp>

IR::InstrBase::InstrBase(){
    static IR::index_t newId = 0;
    index = newId++;
}

const IR::index_t IR::getInstrIndex(const IR::Instrction& instr){
    return std::visit<IR::index_t>([](auto& real){
        return real.index;
    }, instr);
}
