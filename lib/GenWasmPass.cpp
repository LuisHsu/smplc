#include <GenWasmPass.hpp>

#include <fstream>

void GenWasmPass::output(std::string filePrefix){
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
    
    // Write wasm
    fout.open(filePrefix + ".wasm", std::ios::trunc | std::ios::binary);
    // TODO:
    fout.close();
}

void GenWasmPass::beforeParse(Parser::Computation&){
    /* stdio */
    // Type
    module.types.emplace_back(); // For OutputNewLine(), InputNum()
    module.types.emplace_back(Wasm::FuncType {.params = {Wasm::ValueType::i32}}); // For OutputNum(i32)
    // Import
    module.imports.emplace_back(Wasm::Import {.module = "stdio", .name = "OutputNum", .descType = Wasm::DescType::Func, .desc = 1U});
    module.imports.emplace_back(Wasm::Import {.module = "stdio", .name = "InputNum", .descType = Wasm::DescType::Func, .desc = 0U});
    module.imports.emplace_back(Wasm::Import {.module = "stdio", .name = "OutputNewLine", .descType = Wasm::DescType::Func, .desc = 0U});
    funcSerial = 3;
    /* Memory */
    module.mems.emplace_back(1U, 0U);
    /* Main function */
    funcStack.emplace();
}

void GenWasmPass::afterParse(Parser::Computation&){
    // Func main
    module.funcs.emplace_back(funcStack.top());
    funcStack.pop();
    // Export main
    module.exports.emplace_back(Wasm::Export {.name = "main", .descType = Wasm::DescType::Func, .descIdx = funcSerial});
}