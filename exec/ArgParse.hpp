#ifndef SMPLC_ArgParse_DEF
#define SMPLC_ArgParse_DEF

#include <string>
#include <vector>

class ArgParse{
public:
    ArgParse(int argc, char const *argv[]);

    std::vector<std::string> inputFiles;
    bool parserDebug;
};

#endif