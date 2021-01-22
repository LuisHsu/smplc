#ifndef SMPLC_Parser_DEF
#define SMPLC_Parser_DEF

#include <Source.hpp>

namespace Parser{

class Interface{
public:
    Interface(Source& source);
    virtual bool parse() = 0;
protected:
    Source& source;
};

class Letter: public Interface{
public:
    Letter(Source& source);
    bool parse();
};

class Digit: public Interface{
public:
    Digit(Source& source);
    bool parse();
};

}



#endif