#include <GenWasm.hpp>

#include <fstream>

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

void GenWasm::binary(std::string filePrefix){
    // Write wasm
    std::ofstream fout(filePrefix + ".wasm", std::ios::trunc | std::ios::binary);
    // TODO:
    fout.close();
}

