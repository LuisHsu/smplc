#ifndef SMPLC_ArgParse_DEF
#define SMPLC_ArgParse_DEF

#include <string>
#include <vector>
#include <string>
#include <filesystem>

class ArgParse{
public:
    ArgParse(int argc, char const *argv[]);

    std::vector<std::string> inputFiles;
    std::string outputFile;
    bool parserDebug;
    bool parseOnly;
};

#endif