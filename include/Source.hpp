#ifndef SMPLC_Source_DEF
#define SMPLC_Source_DEF

#include <istream>
#include <string>

class Source{
private:
    std::istream& stream;
    int lastChar;
public:
    Source(std::istream& stream);
    int get();
    Source& operator>>(int& obj);
};

#endif