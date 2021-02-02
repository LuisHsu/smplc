#ifndef SMPLC_Parser_DEF
#define SMPLC_Parser_DEF

#include <vector>
#include <Source.hpp>

namespace Parser{

class Pass;

class Interface{
public:
    Interface(Source& source, std::vector<Pass>& passes);
    virtual bool parse() = 0;
protected:
    Source& source;
    std::vector<Pass>& passes;
};

class Letter: public Interface{
public:
    Letter(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Digit: public Interface{
public:
    Digit(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class RelOp: public Interface{
public:
    RelOp(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Ident: public Interface{
public:
    Ident(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Number: public Interface{
public:
    Number(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Designator: public Interface{
public:
    Designator(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Factor: public Interface{
public:
    Factor(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Term: public Interface{
public:
    Term(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Expression: public Interface{
public:
    Expression(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Relation: public Interface{
public:
    Relation(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Assignment: public Interface{
public:
    Assignment(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class FuncCall: public Interface{
public:
    FuncCall(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class ReturnStatement: public Interface{
public:
    ReturnStatement(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Statement: public Interface{
public:
    Statement(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class StatSequence: public Interface{
public:
    StatSequence(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class IfStatement: public Interface{
public:
    IfStatement(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class WhileStatement: public Interface{
public:
    WhileStatement(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class TypeDecl: public Interface{
public:
    TypeDecl(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class VarDecl: public Interface{
public:
    VarDecl(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class FormalParam: public Interface{
public:
    FormalParam(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class FuncBody: public Interface{
public:
    FuncBody(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class FuncDecl: public Interface{
public:
    FuncDecl(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Computation: public Interface{
public:
    Computation(Source& source, std::vector<Pass>& passes);
    bool parse();
};

class Pass{
public:
    void beforeParse(Letter&);
    void beforeParse(Digit&);
    void beforeParse(RelOp&);
    void beforeParse(Ident&);
    void beforeParse(Number&);
    void beforeParse(Designator&);
    void beforeParse(Factor&);
    void beforeParse(Term&);
    void beforeParse(Expression&);
    void beforeParse(Relation&);
    void beforeParse(Assignment&);
    void beforeParse(FuncCall&);
    void beforeParse(ReturnStatement&);
    void beforeParse(Statement&);
    void beforeParse(StatSequence&);
    void beforeParse(IfStatement&);
    void beforeParse(WhileStatement&);
    void beforeParse(TypeDecl&);
    void beforeParse(VarDecl&);
    void beforeParse(FormalParam&);
    void beforeParse(FuncBody&);
    void beforeParse(FuncDecl&);
    void beforeParse(Computation&);
    
    void afterParse(Letter&);
    void afterParse(Digit&);
    void afterParse(RelOp&);
    void afterParse(Ident&);
    void afterParse(Number&);
    void afterParse(Designator&);
    void afterParse(Factor&);
    void afterParse(Term&);
    void afterParse(Expression&);
    void afterParse(Relation&);
    void afterParse(Assignment&);
    void afterParse(FuncCall&);
    void afterParse(ReturnStatement&);
    void afterParse(Statement&);
    void afterParse(StatSequence&);
    void afterParse(IfStatement&);
    void afterParse(WhileStatement&);
    void afterParse(TypeDecl&);
    void afterParse(VarDecl&);
    void afterParse(FormalParam&);
    void afterParse(FuncBody&);
    void afterParse(FuncDecl&);
    void afterParse(Computation&);
};

}

#endif