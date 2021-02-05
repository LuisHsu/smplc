#ifndef SMPLC_PrintPass_DEF
#define SMPLC_PrintPass_DEF

#include <Parser.hpp>

class PrintPass: public Parser::Pass{
public:
    void afterParse(Parser::RelOp&);
    void afterParse(Parser::Ident&);
    void afterParse(Parser::Number&);
    void afterParse(Parser::Designator&);
    void afterParse(Parser::Factor&);
    void afterParse(Parser::Term&);
    void afterParse(Parser::Expression&);
    void afterParse(Parser::Relation&);
    void afterParse(Parser::Assignment&);
    void afterParse(Parser::FuncCall&);
    void afterParse(Parser::IfStatement&);
    void afterParse(Parser::WhileStatement&);
    void afterParse(Parser::ReturnStatement&);
    void afterParse(Parser::Statement&);
    void afterParse(Parser::StatSequence&);
    void afterParse(Parser::TypeDecl&);
    void afterParse(Parser::VarDecl&);
    void afterParse(Parser::FormalParam&);
    void afterParse(Parser::FuncBody&);
};

#endif