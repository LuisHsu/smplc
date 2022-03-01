#ifndef SMPLC_Source_DEF
#define SMPLC_Source_DEF

#include <istream>
#include <string>
#include <stack>

class Source{
private:
    std::istream& stream;
    std::stack<int> back;
public:
    Source(std::istream& stream);
    int get();
    void putback(int);
};

#endif