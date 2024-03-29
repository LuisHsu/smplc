#ifndef SMPLC_Parser_DEF
#define SMPLC_Parser_DEF

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <variant>
#include <optional>
#include <Source.hpp>

namespace Parser{

class Pass;
class Expression;
class Term;
class Statement;

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
    std::string value;
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

class Expression: public Interface{
public:
    Expression(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
    enum class Type{
        None, Plus, Minus
    };
    std::vector<std::pair<Type, Term>> terms;
};

class FuncCall: public Interface{
public:
    FuncCall(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    Ident identifier;
    std::vector<Expression> expressions;
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

    Expression expression;
};

class StatSequence: public Interface{
public:
    StatSequence(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    std::vector<Statement> statements;
};

class IfStatement: public Interface{
public:
    IfStatement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    Relation relation;
    StatSequence thenStat;
    std::optional<StatSequence> elseStat;
};

class WhileStatement: public Interface{
public:
    WhileStatement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    Relation relation;
    StatSequence statements;
};

class Statement: public Interface{
public:
    Statement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    using ValueType = std::variant<
        std::monostate,
        Assignment,
        FuncCall,
        IfStatement,
        WhileStatement,
        ReturnStatement
    >;
    ValueType value;
};

class TypeDecl: public Interface{
public:
    TypeDecl(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();
    enum class Type{
        None, Variable, Array
    };
    Type declType;
    std::vector<Number> arraySizes;
};

class VarDecl: public Interface{
public:
    VarDecl(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    TypeDecl typeDecl;
    std::vector<Ident> identifiers;
};

class FormalParam: public Interface{
public:
    FormalParam(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    std::vector<Ident> identifiers;
};

class FuncBody: public Interface{
public:
    FuncBody(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    std::vector<VarDecl> varDecls;
    std::optional<StatSequence> statSequence;
};

class FuncDecl: public Interface{
public:
    FuncDecl(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    bool isVoid;
    Ident identifier;
    FormalParam parameter;
    FuncBody body;
};

class Computation: public Interface{
public:
    Computation(Source& source, std::vector<std::reference_wrapper<Pass>>& passes);
    bool parse();

    std::vector<VarDecl> varDecls;
    std::vector<FuncDecl> funcDecls;
    StatSequence statement;
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