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

class RelOp: public Interface{
public:
    RelOp(Source& source);
    bool parse();
};

class Ident: public Interface{
public:
    Ident(Source& source);
    bool parse();
};

class Number: public Interface{
public:
    Number(Source& source);
    bool parse();
};

class Designator: public Interface{
public:
    Designator(Source& source);
    bool parse();
};

class Factor: public Interface{
public:
    Factor(Source& source);
    bool parse();
};

class Term: public Interface{
public:
    Term(Source& source);
    bool parse();
};

class Expression: public Interface{
public:
    Expression(Source& source);
    bool parse();
};

class Relation: public Interface{
public:
    Relation(Source& source);
    bool parse();
};

class Assignment: public Interface{
public:
    Assignment(Source& source);
    bool parse();
};

class FuncCall: public Interface{
public:
    FuncCall(Source& source);
    bool parse();
};

}



#endif