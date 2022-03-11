#include <EmitWasmPass.hpp>

#include <fstream>
#include <Logger.hpp>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

uint32_t EmitWasmPass::FuncEntry::serial = 3;

EmitWasmPass::FuncEntry::FuncEntry():id(serial++){}

void EmitWasmPass::beforeParse(Parser::Computation&){
    /* stdio */
    // Type
    module.types.emplace_back(); // For OutputNewLine(), InputNum()
    module.types.emplace_back(Wasm::FuncType {.params = {Wasm::ValueType::i32}}); // For OutputNum(i32)
    // Import
    module.imports.emplace_back(Wasm::Import {.module = "stdio", .name = "OutputNum", .desc = 1U});
    module.imports.emplace_back(Wasm::Import {.module = "stdio", .name = "InputNum", .desc = 0U});
    module.imports.emplace_back(Wasm::Import {.module = "stdio", .name = "OutputNewLine", .desc = 0U});
    /* Memory */
    module.mems.emplace_back(1U, std::nullopt);
    /* Main function */
    funcStack.emplace();
}

void EmitWasmPass::afterParse(Parser::Computation&){
    // Func main
    FuncEntry& entry = funcStack.top();
    module.funcs.emplace_back(entry.func);
    // Export main
    module.exports.emplace_back(Wasm::Export {.name = "main", .descType = Wasm::DescType::Func, .descIdx = entry.id});
    funcStack.pop();
}

void EmitWasmPass::afterParse(Parser::VarDecl& decl){
    for(Parser::Ident& ident: decl.identifiers){
        FuncEntry& entry = funcStack.top();
        if(entry.identMap.contains(ident.value)){
            Logger::put(LogLevel::Error, std::string("identifier '" + ident.value + "' has been defined"));
        }else{
            entry.identMap.emplace(ident.value, entry.identMap.size());
            entry.func.locals.emplace_back(Wasm::ValueType::i32);
        }
    }
}

void EmitWasmPass::afterParse(Parser::StatSequence&){
    funcStack.top().func.body.emplace_back(Wasm::Instr_end());
}

void EmitWasmPass::afterParse(Parser::Factor& factor){
    std::visit(overload {
        [&](Parser::Designator&){
            // TODO:
        },
        [&](Parser::Number& num){
            Wasm::Instr_i32_const instr {.value = (int32_t)num.value};
            funcStack.top().func.body.emplace_back(instr);
        },
        [&](Parser::Expression&){
            // TODO:
        },
        [&](Parser::FuncCall&){
            // TODO:
        },
        [&](std::monostate&){
        }
    }, factor.value);
}