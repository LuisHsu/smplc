#include <GenWasm.hpp>
#include <Wasm.hpp>

#include <fstream>
#include <sstream>
#include <vector>
#include <cstddef>

static std::ostream& operator<<(std::ostream& out, Wasm::FuncType& module);
static std::ostream& operator<<(std::ostream& out, Wasm::Module& module);

void GenWasm::wrapper(std::string filePrefix){
    // Write js wrapper
    std::ofstream fout(filePrefix + ".js", std::ios::trunc);
    fout << "const fs = require('fs')\n"
        "const buf = Buffer.alloc(50);\n"
        "const cin = fs.openSync(\"/dev/stdin\", \"rs\");\n"
        "WebAssembly.instantiate(fs.readFileSync(\"" + filePrefix + ".wasm\"), {\n"
        "    \"stdio\":{\n"
        "    \"OutputNum\": (num) => {\n"
        "        console.log(num);\n"
        "    },\n"
        "    \"InputNum\": () => {\n"
        "        fs.readSync(cin, buf)\n"
        "        return  Number.parseInt(buf.toString())\n"
        "    },\n"
        "    \"OutputNewLine\" : () => {\n"
        "        console.log()\n"
        "    }\n"
        "}\n"
        "})\n"
        ".then(({instance}) => {\n"
        "    instance.exports.main();\n"
        "})"
    << std::endl;
    fout.close();
}

void GenWasm::binary(std::string filePrefix, Wasm::Module& module){
    std::ofstream fout(filePrefix + ".wasm", std::ios::trunc | std::ios::binary);
    fout << module;
    fout.close();
}

static std::vector<std::byte> leb128(uint32_t value){
    std::vector<std::byte> result;
    for(int i = 0; i < 5; ++i){
        std::byte byte {(uint8_t)(value & 0x7f)};
        if((value >>= 7) != 0){
            byte |= std::byte{0x80};
            result.emplace_back(byte | (std::byte)0x80);
        }else{
            result.emplace_back(byte);
            break;
        }
    }
    return result;
}

static std::vector<std::byte> leb128(int32_t value){
    return leb128((uint32_t)value);
}

template<uint8_t N, class T, class A>
static void writeSection(std::ostream& out, std::vector<T, A>& instances){
    static const std::byte sectNum {N};
    std::stringstream stream(std::ios::binary);
    for(T& instance: instances){
        stream << instance;
    }
    std::string section(stream.str());
    out.write((char*)&sectNum, 1);
    std::vector<std::byte> sectSize = leb128((uint32_t)section.size());
    out.write((char*)sectSize.data(), sectSize.size());
    out.write(section.data(), section.size());
}

static std::ostream& operator<<(std::ostream& out, Wasm::Module& module){
    // Magic & version
    out.write("\00asm" "\x01\x00\x00\x00", 8);
    // Type section
    writeSection<0x01>(out, module.types);
    // return
    return out;
}

static std::ostream& operator<<(std::ostream& out, Wasm::FuncType& module){
    // return
    return out;
}