#include <EmitWasmPass.hpp>

#include <fstream>
#include <algorithm>
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

void EmitWasmPass::afterParse(Parser::Computation& computation){
    if(computation.isSuccess){
        // Func main
        FuncEntry& entry = funcStack.top();
        module.funcs.emplace_back(entry.func);
        // Export main
        module.exports.emplace_back(Wasm::Export {.name = "main", .descType = Wasm::DescType::Func, .descIdx = entry.id});
        funcStack.pop();
    }
}

void EmitWasmPass::afterParse(Parser::VarDecl& decl){
    if(decl.isSuccess){
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
}

void EmitWasmPass::afterParse(Parser::StatSequence& statSeq){
    if(statSeq.isSuccess){
        funcStack.top().func.body.emplace_back(Wasm::Instr_end());
    }
}

void EmitWasmPass::afterParse(Parser::Factor& factor){
    if(factor.isSuccess){
        std::visit(overload {
            [&](Parser::Designator& designator){
                if(funcStack.top().identMap.contains(designator.identifier.value)){
                    instrStack.top().emplace(Wasm::Instr_local_get {.index = funcStack.top().identMap[designator.identifier.value]});
                }else{
                    Logger::put(LogLevel::Error, std::string("undefined identifier '" + designator.identifier.value + "'"));
                }
            },
            [&](Parser::Number& num){
                instrStack.top().emplace(Wasm::Instr_i32_const {.value = (int32_t)num.value});
            },
            [&](Parser::FuncCall&){
                // TODO:
            },
            [](auto&){},
        }, factor.value);
    }
}

void EmitWasmPass::afterParse(Parser::Term& term){
    if(term.isSuccess){
        std::queue<Wasm::Instr> instrs;
        for(auto it = term.factors.begin(); it != term.factors.end(); it = std::next(it)){
            if(std::holds_alternative<Wasm::Instr_nop>(instrStack.top().front())){
                instrStack.top().pop();
                while(!std::holds_alternative<Wasm::Instr_end>(instrStack.top().front())){
                    instrs.emplace(instrStack.top().front());
                    instrStack.top().pop();
                }
            }else{
                instrs.emplace(instrStack.top().front());
            }
            instrStack.top().pop();
            switch (it->first){
            case Parser::Term::Type::Times:
                instrs.emplace(Wasm::Instr_i32_mul());
                break;
            case Parser::Term::Type::Divide:
                instrs.emplace(Wasm::Instr_i32_div_s());
                break;
            default:
                break;
            }
        }
        instrStack.pop();
        while(!instrs.empty()){
            instrStack.top().emplace(instrs.front());
            instrs.pop();
        }
        instrStack.top().emplace(Wasm::Instr_end());
    }
}

void EmitWasmPass::afterParse(Parser::Expression& expression){
    if(expression.isSuccess){
        std::queue<Wasm::Instr> instrs;
        for(auto it = expression.terms.begin(); it != expression.terms.end(); it = std::next(it)){
            if(std::holds_alternative<Wasm::Instr_nop>(instrStack.top().front())){
                instrStack.top().pop();
                while(!std::holds_alternative<Wasm::Instr_end>(instrStack.top().front())){
                    instrs.emplace(instrStack.top().front());
                    instrStack.top().pop();
                }
                instrStack.top().pop();
            }else{
                instrs.emplace(instrStack.top().front());
            }
            switch (it->first){
            case Parser::Expression::Type::Plus:
                instrs.emplace(Wasm::Instr_i32_add());
                break;
            case Parser::Expression::Type::Minus:
                instrs.emplace(Wasm::Instr_i32_sub());
                break;
            default:
                break;
            }
        }
        instrStack.pop();
        while(!instrs.empty()){
            instrStack.top().emplace(instrs.front());
            instrs.pop();
        }
        instrStack.top().emplace(Wasm::Instr_end());
    }
}

void EmitWasmPass::afterParse(Parser::Assignment& assignment){
    if(assignment.isSuccess){
        std::queue<Wasm::Instr>& instrs = instrStack.top();
        instrs.pop();
        while(instrs.size() > 1){
            funcStack.top().func.body.emplace_back(instrs.front());
            instrs.pop();
        }
        instrStack.pop();
        if(funcStack.top().identMap.contains(assignment.designator.identifier.value)){
            funcStack.top().func.body.emplace_back(Wasm::Instr_local_set {.index = funcStack.top().identMap[assignment.designator.identifier.value]});
        }else{
            Logger::put(LogLevel::Error, std::string("undefined identifier '" + assignment.designator.identifier.value + "'"));
        }
    }
}

void EmitWasmPass::beforeParse(Parser::Expression&){
    instrStack.top().emplace(Wasm::Instr_nop());
    instrStack.emplace();
}

void EmitWasmPass::beforeParse(Parser::Term&){
    instrStack.top().emplace(Wasm::Instr_nop());
    instrStack.emplace();
}

void EmitWasmPass::beforeParse(Parser::Assignment&){
    instrStack.emplace();
}