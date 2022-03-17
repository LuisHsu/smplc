#include <GenWasm.hpp>
#include <Wasm.hpp>

#include <fstream>
#include <vector>
#include <cstddef>
#include <variant>
#include <numeric>

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
static std::ostream& operator<<(std::ostream&, Wasm::Global&);
static std::ostream& operator<<(std::ostream&, Wasm::Func&);
static std::ostream& operator<<(std::ostream&, Wasm::ConstExpr&);

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
            if((i < 4) && (byte > (std::byte)0x40)){
                bytes.emplace_back(byte | (std::byte)0x80);
                bytes.emplace_back((std::byte)0x00);
            }else{
                bytes.emplace_back(byte);
            }
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
    if(instances.size() > 0){
        Section section;
        std::ostream stream(&section);
        writeInt(stream, (uint16_t)instances.size());
        for(T& instance: instances){
            stream << instance;
        }
        return out << std::byte(N) << section;
    }
    return out;
}

template<class T, class A>
static std::ostream& operator<<(std::ostream& out, std::vector<T, A>& instances){
    writeInt(out, (uint32_t)instances.size());
    for(T& instance: instances){
        out << instance;
    }
    return out;
}

static std::ostream& funcSection(std::ostream& out, std::vector<Wasm::Func>& funcs){
    Section section;
    std::ostream stream(&section);
    writeInt(stream, (uint16_t)funcs.size());
    for(Wasm::Func& func: funcs){
        writeInt(stream, func.type);
    }
    return out << std::byte(0x03) << section;
}

static std::ostream& operator<<(std::ostream& out, Wasm::Module& module){
    // Magic & version
    out.write("\00asm" "\x01\x00\x00\x00", 8);
    writeSection<0x01>(out, module.types); // Type
    writeSection<0x02>(out, module.imports); // Import
    funcSection(out, module.funcs); // Func
    writeSection<0x04>(out, module.tables); // Table
    writeSection<0x05>(out, module.mems); // Memory
    writeSection<0x06>(out, module.globals); // Global
    // TODO: Export section
    // TODO: Start section
    // TODO: Elem section
    writeSection<0x0A>(out, module.funcs); // Code
    // TODO: Data section
    // TODO: Data count section
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

static std::ostream& operator<<(std::ostream& out, Wasm::Global& global){
    return out << (Wasm::GlobalType&)global << global.init;
}

static std::ostream& operator<<(std::ostream& out, Wasm::ConstExpr& expr){
    std::visit(overload{
        [&](int32_t& value){
            out << std::byte(Wasm::Instr_i32_const::opcode);
            writeInt(out, value);
        },
        [&](int64_t& value){
            out << std::byte(Wasm::Instr_i64_const::opcode);
            writeInt(out, value);
        },
        [&](float32_t value){
            out << std::byte(Wasm::Instr_f32_const::opcode);
            std::byte* data = (std::byte*)&value;
            for(int i = 0; i < 4; ++i){
                out << data[i];
            }
        },
        [&](float64_t value){
            out << std::byte(Wasm::Instr_f64_const::opcode);
            std::byte* data = (std::byte*)&value;
            for(int i = 0; i < 8; ++i){
                out << data[i];
            }
        }
    }, expr.value);
    out << std::byte(Wasm::Instr_end::opcode);
    return out;
}

static std::ostream& operator<<(std::ostream& out, Wasm::Func& func){
    Section code;
    std::ostream stream(&code);
    // Locals
    std::vector<std::pair<Wasm::ValueType, uint32_t>> localPair;
    localPair.emplace_back(std::reduce(func.locals.begin(), func.locals.end(),
        std::pair<Wasm::ValueType, uint32_t> {func.locals.front(), 0}, 
        [&](std::pair<Wasm::ValueType, uint32_t> acc, Wasm::ValueType& local) -> std::pair<Wasm::ValueType, uint32_t>{
            if(local == acc.first){
                return std::pair<Wasm::ValueType, uint32_t> {local, acc.second + 1};
            }else{
                localPair.emplace_back(acc);
                return std::pair<Wasm::ValueType, uint32_t> {local, 1};
            }
        }
    ));
    writeInt(stream, (uint32_t)localPair.size());
    for(std::pair<Wasm::ValueType, uint32_t>& local: localPair){
        writeInt(stream, local.second);
        stream << local.first;
    }
    // Instructions
    for(Wasm::Instr& instr: func.body){
        std::visit(overload {
            [&](Wasm::Instr_i32_const& instr){
                stream << std::byte(instr.opcode);
                writeInt(stream, instr.value);
            },
            [&](Wasm::Instr_if& instr){
                stream << std::byte(instr.opcode);
                std::visit(overload{
                    [&](std::monostate&){
                        stream << std::byte(0x40);
                    },
                    [&](uint32_t& index){
                        writeInt(stream, index);
                    },
                    [&](Wasm::ValueType& type){
                        stream << type;
                    }
                }, instr.blocktype);
            },
            [&](Wasm::Instr_loop& instr){
                stream << std::byte(instr.opcode);
                std::visit(overload{
                    [&](std::monostate&){
                        stream << std::byte(0x40);
                    },
                    [&](uint32_t& index){
                        writeInt(stream, index);
                    },
                    [&](Wasm::ValueType& type){
                        stream << type;
                    }
                }, instr.blocktype);
            },
            [&](Wasm::Instr_local_get& instr){
                stream << std::byte(instr.opcode);
                writeInt(stream, instr.index);
            },
            [&](Wasm::Instr_local_set& instr){
                stream << std::byte(instr.opcode);
                writeInt(stream, instr.index);
            },
            [&](auto& instr){
                stream << std::byte(instr.opcode);
            },
        }, instr);
    }
    return out << code;
}