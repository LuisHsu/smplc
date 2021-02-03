#ifndef SMPLC_Parser_DEF
#define SMPLC_Parser_DEF

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <variant>
#include <Source.hpp>

namespace Parser{

class Pass;
class Expression;
class Term;

class Interface{
public:
    Interface(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    virtual bool parse() = 0;
    bool isSuccess;
protected:
    Source& source;
    std::vector<std::reference_wrapper<Pass>>& passes;
};

class Letter: public Interface{
public:
    Letter(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
    int letter;
};

class Digit: public Interface{
public:
    Digit(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
    int digit;
};

class RelOp: public Interface{
public:
    enum class Type{
        Equal, NonEqual, GreaterThan, GreaterEqual, LessThan, LessEqual
    };
    RelOp(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
    Type opType;
};

class Ident: public Interface{
public:
    Ident(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
    std::string identifier;
};

class Number: public Interface{
public:
    Number(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
    unsigned long long int value;
};

class Designator: public Interface{
public:
    Designator(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
    Ident identifier;
    std::vector<Expression> expressions;
};

class FuncCall: public Interface{
public:
    FuncCall(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class Expression: public Interface{
public:
    Expression(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
    enum class Type{
        None, Plus, Minus
    };
    std::vector<std::pair<Type, Term>> terms;
};

class Assignment: public Interface{
public:
    Assignment(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    Designator designator;
    Expression expression;
};

class Relation: public Interface{
public:
    Relation(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    RelOp::Type opType;
    Expression leftExpr, rightExpr;
};

class Factor: public Interface{
public:
    Factor(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    using ValueType = std::variant<std::monostate, Designator, Number, Expression, FuncCall>;
    ValueType value;
};

class Term: public Interface{
public:
    Term(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    enum class Type{
        None, Times, Divide
    };
    std::vector<std::pair<Type, Factor>> factors;
};

class ReturnStatement: public Interface{
public:
    ReturnStatement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class Statement: public Interface{
public:
    Statement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class StatSequence: public Interface{
public:
    StatSequence(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class IfStatement: public Interface{
public:
    IfStatement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class WhileStatement: public Interface{
public:
    WhileStatement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class TypeDecl: public Interface{
public:
    TypeDecl(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class VarDecl: public Interface{
public:
    VarDecl(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class FormalParam: public Interface{
public:
    FormalParam(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class FuncBody: public Interface{
public:
    FuncBody(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class FuncDecl: public Interface{
public:
    FuncDecl(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class Computation: public Interface{
public:
    Computation(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
};

class Pass{
public:
    virtual void beforeParse(Letter&);
    virtual void beforeParse(Digit&);
    virtual void beforeParse(RelOp&);
    virtual void beforeParse(Ident&);
    virtual void beforeParse(Number&);
    virtual void beforeParse(Designator&);
    virtual void beforeParse(Factor&);
    virtual void beforeParse(Term&);
    virtual void beforeParse(Expression&);
    virtual void beforeParse(Relation&);
    virtual void beforeParse(Assignment&);
    virtual void beforeParse(FuncCall&);
    virtual void beforeParse(ReturnStatement&);
    virtual void beforeParse(Statement&);
    virtual void beforeParse(StatSequence&);
    virtual void beforeParse(IfStatement&);
    virtual void beforeParse(WhileStatement&);
    virtual void beforeParse(TypeDecl&);
    virtual void beforeParse(VarDecl&);
    virtual void beforeParse(FormalParam&);
    virtual void beforeParse(FuncBody&);
    virtual void beforeParse(FuncDecl&);
    virtual void beforeParse(Computation&);
    
    virtual void afterParse(Letter&);
    virtual void afterParse(Digit&);
    virtual void afterParse(RelOp&);
    virtual void afterParse(Ident&);
    virtual void afterParse(Number&);
    virtual void afterParse(Designator&);
    virtual void afterParse(Factor&);
    virtual void afterParse(Term&);
    virtual void afterParse(Expression&);
    virtual void afterParse(Relation&);
    virtual void afterParse(Assignment&);
    virtual void afterParse(FuncCall&);
    virtual void afterParse(ReturnStatement&);
    virtual void afterParse(Statement&);
    virtual void afterParse(StatSequence&);
    virtual void afterParse(IfStatement&);
    virtual void afterParse(WhileStatement&);
    virtual void afterParse(TypeDecl&);
    virtual void afterParse(VarDecl&);
    virtual void afterParse(FormalParam&);
    virtual void afterParse(FuncBody&);
    virtual void afterParse(FuncDecl&);
    virtual void afterParse(Computation&);
};

}

#endif