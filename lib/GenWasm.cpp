#include <GenWasm.hpp>
#include <Wasm.hpp>

#include <fstream>
#include <vector>
#include <cstddef>
#include <variant>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

class Section;
static std::ostream& writeInt(std::ostream&, uint32_t);
static std::ostream& writeInt(std::ostream&, uint64_t);
static std::ostream& writeInt(std::ostream&, int32_t);
static std::ostream& writeInt(std::ostream&, int64_t);
static std::ostream& operator<<(std::ostream&, std::byte);
static std::ostream& operator<<(std::ostream&, Section&);
static std::ostream& operator<<(std::ostream&, Wasm::Module&);
static std::ostream& operator<<(std::ostream&, Wasm::FuncType&);
static std::ostream& operator<<(std::ostream&, Wasm::ValueType&);
static std::ostream& operator<<(std::ostream&, Wasm::Import&);
static std::ostream& operator<<(std::ostream&, Wasm::Table&);
static std::ostream& operator<<(std::ostream&, Wasm::Limit&);
static std::ostream& operator<<(std::ostream&, Wasm::GlobalType&);
static std::ostream& operator<<(std::ostream&, Wasm::Ref::Type&);

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

class Section: public std::streambuf{
public:
    std::vector<std::byte> bytes;
protected:
    int overflow(int c){
        return (int)bytes.emplace_back((std::byte)c);
    }
friend std::ostream& operator<<(std::ostream&, Section&);
};

static std::ostream& operator<<(std::ostream& out, Section& section){
    return writeInt(out, (uint32_t)section.bytes.size()).write((char*)section.bytes.data(), section.bytes.size());
}

static std::ostream& writeInt(std::ostream& out, uint32_t value){
    std::vector<std::byte> bytes;
    for(int i = 0; i < 5; ++i){
        std::byte byte {(uint8_t)(value & 0x7f)};
        if((value >>= 7) != 0){
            byte |= std::byte{0x80};
            bytes.emplace_back(byte | (std::byte)0x80);
        }else{
            bytes.emplace_back(byte);
            break;
        }
    }
    return out.write((char*)bytes.data(), bytes.size());
}

static std::ostream& writeInt(std::ostream& out, uint64_t value){
    std::vector<std::byte> bytes;
    for(int i = 0; i < 10; ++i){
        std::byte byte {(uint8_t)(value & 0x7f)};
        if((value >>= 7) != 0){
            byte |= std::byte{0x80};
            bytes.emplace_back(byte | (std::byte)0x80);
        }else{
            bytes.emplace_back(byte);
            break;
        }
    }
    return out.write((char*)bytes.data(), bytes.size());
}

static std::ostream& writeInt(std::ostream& out, int32_t value){
    return writeInt(out, (uint32_t)value);
}

static std::ostream& writeInt(std::ostream& out, int64_t value){
    return writeInt(out, (uint64_t)value);
}

static std::ostream& operator<<(std::ostream& out, std::byte value){
    return out.write((char*)&value, 1);
}

template<uint8_t N, class T, class A>
static std::ostream& writeSection(std::ostream& out, std::vector<T, A>& instances){
    Section section;
    std::ostream stream(&section);
    writeInt(stream, (uint16_t)instances.size());
    for(T& instance: instances){
        stream << instance;
    }
    return out << std::byte(N) << section;
}

template<class T, class A>
static std::ostream& operator<<(std::ostream& out, std::vector<T, A>& instances){
    writeInt(out, (uint32_t)instances.size());
    for(T& instance: instances){
        out << instance;
    }
    return out;
}

static std::ostream& operator<<(std::ostream& out, Wasm::Module& module){
    // Magic & version
    out.write("\00asm" "\x01\x00\x00\x00", 8);
    writeSection<0x01>(out, module.types); // Type
    writeSection<0x02>(out, module.imports); // Import
    // TODO: Func section
    // TODO: Table section
    // TODO: Memory section
    // TODO: Global section
    // TODO: Export section
    // TODO: Start section
    // TODO: Elem section
    // TODO: Data section
    // return
    return out;
}

static std::ostream& operator<<(std::ostream& out, Wasm::ValueType& type){
    switch(type){
        case Wasm::ValueType::i32:
            return out << std::byte(0x7f);
        case Wasm::ValueType::i64:
            return out << std::byte(0x7e);
        case Wasm::ValueType::f32:
            return out << std::byte(0x7d);
        case Wasm::ValueType::f64:
            return out << std::byte(0x7c);
    }
}

static std::ostream& operator<<(std::ostream& out, Wasm::FuncType& funcType){
    return out << std::byte(0x60) << funcType.params << funcType.results;
}

static std::ostream& operator<<(std::ostream& out, Wasm::Import& import){
    writeInt(out, (uint32_t)import.module.size());
    out.write(import.module.data(), import.module.size());
    writeInt(out, (uint32_t)import.name.size());
    out.write(import.name.data(), import.name.size());
    return std::visit(overload {
        [&](uint32_t& index) -> std::ostream& {
            out << std::byte(0x00);
            return writeInt(out, index);
        },
        [&](Wasm::Table& table) -> std::ostream& {
            return out << std::byte(0x01) << table;
        },
        [&](Wasm::Memory& mem) -> std::ostream& {
            return out << std::byte(0x02) << mem;
        },
        [&](Wasm::GlobalType& global) -> std::ostream& {
            return out << std::byte(0x03) << global;
        },
    }, import.desc);
}

static std::ostream& operator<<(std::ostream& out, Wasm::Table& table){
    return out << table.refType << table.limits;
}

static std::ostream& operator<<(std::ostream& out, Wasm::Limit& limit){
    if(limit.second.has_value()){
        out << std::byte(0x01);
        writeInt(out, limit.first);
        writeInt(out, limit.second.value());
    }else{
        out << std::byte(0x00);
        writeInt(out, limit.first);
    }
    return out;
}

static std::ostream& operator<<(std::ostream& out, Wasm::GlobalType& type){
    return out << type.valueType << std::byte(type.mut ? 0x01 : 0x00);
}

static std::ostream& operator<<(std::ostream& out, Wasm::Ref::Type& type){
    switch(type){
        case Wasm::Ref::Type::Func:
            return out << std::byte(0x70);
        case Wasm::Ref::Type::Extern:
            return out << std::byte(0x6f);
    }
}