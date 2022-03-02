#ifndef SMPLC_GenWasm_DEF
#define SMPLC_GenWasm_DEF

#include <Wasm.hpp>
#include <string>
#include <unordered_map>

namespace GenWasm{
    void wrapper(std::string filePrefix);
    void binary(std::string filePrefix);
}

#endif