/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <Parser.hpp>
#include <Logger.hpp>

template<char ch, char... rem>
static unsigned int matchSequence(Source& source){
    int current = source.get();
    unsigned int res = 0;
    if(current == ch){
        if constexpr (sizeof...(rem) > 0){
            res += 1;
            if((res += matchSequence<rem...>(source)) == (sizeof...(rem) + 1)){
                return res;
            }
        }else{
            return 1;
        }
    }
    source.putback(current);
    return res;
}

template<char lower, char upper>
static bool matchRange(Source& source, int& ret){
    ret = source.get();
    if((ret >= lower) && (ret <= upper)){
        return true;
    }else{
        source.putback(ret);
        ret = 0;
        return false;
    }
}

template<char target>
static bool matchOne(Source& source){
    int current = source.get();
    if(current == target){
        return true;
    }else{
        source.putback(current);
        return false;
    }
}

static bool skipWhiteSpaces(Source& source){
    int current;
    while((current = source.get()) == ' ');
    source.putback(current);
    return true;
}

static bool errorOnFalse(bool result, std::string errorMsg){
    if(!result){
        Logger::put(LogLevel::Error, std::string("parse error: ") + errorMsg);
    }
    return result;
}

static bool errorOnPartial(unsigned int result, unsigned int expectedLen, std::string errorMsg){
    if(result > 0 && result != expectedLen){
        Logger::put(LogLevel::Error, std::string("parse error: ") + errorMsg);
    }
    return result == expectedLen;
}

template<typename T>
static void runPassBeforeParse(T& target,std::vector<std::reference_wrapper<Parser::Pass>>& passes){
    for(Parser::Pass& pass : passes){
        pass.beforeParse(target);
    }
}

template<typename T>
static bool runPassAfterParse(bool result, T& target,std::vector<std::reference_wrapper<Parser::Pass>>& passes){
    for(Parser::Pass& pass : passes){
        pass.afterParse(target);
    }
    return result;
}

Parser::Interface::Interface(Source& source, std::vector<std::reference_wrapper<Pass>>& passes):
    source(source), passes(passes), isSuccess(false)
{}

Parser::Letter::Letter(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::Letter::parse(){
    runPassBeforeParse(*this, passes);
    return runPassAfterParse(
        (isSuccess = (matchRange<'a', 'z'>(source, letter) || matchRange<'A', 'Z'>(source, letter))),
        *this, passes
    );
}

Parser::Digit::Digit(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::Digit::parse(){
    runPassBeforeParse(*this, passes);
    return runPassAfterParse(
        (isSuccess = matchRange<'0', '9'>(source, digit)),
        *this, passes
    );
}

Parser::RelOp::RelOp(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::RelOp::parse(){
    runPassBeforeParse(*this, passes);
    if(matchSequence<'=', '='>(source) == 2){
        opType = Parser::RelOp::Type::Equal;
        isSuccess = true;
    }else if(matchSequence<'!', '='>(source) == 2){
        opType = Parser::RelOp::Type::NonEqual;
        isSuccess = true;
    }else if(matchSequence<'>', '='>(source) == 2){
        opType = Parser::RelOp::Type::GreaterEqual;
        isSuccess = true;
    }else if(matchSequence<'<', '='>(source) == 2){
        opType = Parser::RelOp::Type::LessEqual;
        isSuccess = true;
    }else if(matchOne<'>'>(source)){
        opType = Parser::RelOp::Type::GreaterThan;
        isSuccess = true;
    }else if(matchOne<'<'>(source)){
        opType = Parser::RelOp::Type::LessThan;
        isSuccess = true;
    }else{
        isSuccess = false;
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::Ident::Ident(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::Ident::parse(){
    runPassBeforeParse(*this, passes);
    Letter letterObj(source, passes);
    isSuccess = false;
    if(letterObj.parse()){
        identifier = std::string({(char)letterObj.letter});
        Digit digitObj(source, passes);
        while(letterObj.parse() || digitObj.parse()){
            if(letterObj.isSuccess){
                identifier += (char) letterObj.letter;
            }else if(digitObj.isSuccess){
                identifier += (char) digitObj.digit;
            }
        }
        isSuccess = true;
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::Number::Number(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::Number::parse(){
    runPassBeforeParse(*this, passes);
    Digit digitObj(source, passes);
    isSuccess = false;
    if(digitObj.parse()){
        value = digitObj.digit - '0';
        while(digitObj.parse()){
            value = value * 10 + (digitObj.digit - '0');
        }
        isSuccess = true;
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::Designator::Designator(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    identifier(source, passes)
{}
bool Parser::Designator::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    expressions.clear();
    if(identifier.parse()){
        while(skipWhiteSpaces(source) && matchOne<'['>(source)){
            Expression expression(source, passes);
            if(!(
                skipWhiteSpaces(source)
                && expression.parse()
                && skipWhiteSpaces(source)
                && matchOne<']'>(source)
            )){
                return runPassAfterParse(false, *this, passes);
            }
            expressions.push_back(expression);
        }
        expressions.shrink_to_fit();
        isSuccess = true;
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::Factor::Factor(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::Factor::parse(){
    runPassBeforeParse(*this, passes);
    if(value.emplace<FuncCall>(source, passes).parse() ||
        (
            matchOne<'('>(source)
            && skipWhiteSpaces(source)
            && value.emplace<Expression>(source, passes).parse()
            && skipWhiteSpaces(source)
            && matchOne<')'>(source)
        ) || 
        value.emplace<Designator>(source, passes).parse() ||
        value.emplace<Number>(source, passes).parse()
    ){
        isSuccess = true;
    }else{
        isSuccess = false;
        value.emplace<std::monostate>();
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::Term::Term(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::Term::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    factors.clear();
    Factor factor(source, passes);
    if(factor.parse()){
        Type opType = Type::None;
        factors.emplace_back(opType, factor);
        while(
            skipWhiteSpaces(source)
            && (
                (matchOne<'*'>(source) && (opType = Type::Times) == Type::Times) ||
                (matchOne<'/'>(source) && (opType = Type::Divide) == Type::Divide)
            ) && skipWhiteSpaces(source)
            && factor.parse()
        ){
            factors.emplace_back(opType, factor);
        }
        isSuccess = true;
    }
    factors.shrink_to_fit();
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::Expression::Expression(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::Expression::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    terms.clear();
    Term term(source, passes);
    if(term.parse()){
        Type opType = Type::None;
        terms.emplace_back(opType, term);
        while(
            skipWhiteSpaces(source)
            && (
                (matchOne<'+'>(source) && (opType = Type::Plus) == Type::Plus) ||
                (matchOne<'-'>(source) && (opType = Type::Minus) == Type::Minus)
            ) && skipWhiteSpaces(source)
            && term.parse()
        ){
            terms.emplace_back(opType, term);
        }
        isSuccess = true;
    }
    terms.shrink_to_fit();
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::Relation::Relation(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    leftExpr(source, passes), rightExpr(source, passes)
{}
bool Parser::Relation::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    RelOp relOp(source, passes);
    if(
        errorOnFalse(leftExpr.parse(),
            "expected left expression in relation"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && relOp.parse(),
            "expected relation operator in relation"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && rightExpr.parse(),
            "expected right expression in relation"
        )
    ){
        isSuccess = true;
        opType = relOp.opType;
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::Assignment::Assignment(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    designator(source, passes), expression(source, passes)
{}
bool Parser::Assignment::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    return runPassAfterParse((isSuccess = (
        errorOnPartial(matchSequence<'l', 'e', 't'>(source), 3,
            "expected 'let' in assignment"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && designator.parse(),
            "expected designator in assignment"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && (matchSequence<'<', '-'>(source) == 2),
            "expected '<-' in assignment"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && expression.parse(),
            "expected expression in assignment"
        )
    )), *this, passes);
}

Parser::FuncCall::FuncCall(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    identifier(source, passes)
{}
bool Parser::FuncCall::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    expressions.clear();
    if(
        errorOnPartial(matchSequence<'c', 'a', 'l', 'l'>(source), 4,
            "expected 'call' in function call"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && identifier.parse(),
            "expected function name in function call"
        )
    ){
        if(skipWhiteSpaces(source) && matchOne<'('>(source)){
            Expression expr(source, passes);
            if(skipWhiteSpaces(source) && expr.parse()){
                expressions.emplace_back(expr);
                while(skipWhiteSpaces(source) && matchOne<','>(source)){
                    if(errorOnFalse(
                        skipWhiteSpaces(source) && expr.parse(),
                        "expected expression after ',' in function arguments"
                    )){
                        expressions.emplace_back(expr);
                    }else{
                        return runPassAfterParse((isSuccess = false), *this, passes);
                    }
                };
            }
            return runPassAfterParse((isSuccess = errorOnFalse(
                skipWhiteSpaces(source) && matchOne<')'>(source),
                "expected ')' after function arguments"
            )), *this, passes);
        }
        isSuccess = true;
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::ReturnStatement::ReturnStatement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    expression(source, passes)
{}
bool Parser::ReturnStatement::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    if(errorOnPartial(matchSequence<'r', 'e', 't', 'u', 'r', 'n'>(source), 6,
        "expected 'return' in return statement"
    )){
        skipWhiteSpaces(source);
        expression.parse();
        isSuccess = true;
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::Statement::Statement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::Statement::parse(){
    runPassBeforeParse(*this, passes);
    if(
        value.emplace<Assignment>(source, passes).parse() ||
        value.emplace<FuncCall>(source, passes).parse() || 
        value.emplace<ReturnStatement>(source, passes).parse() ||
        value.emplace<IfStatement>(source, passes).parse() ||
        value.emplace<WhileStatement>(source, passes).parse()
    ){
        isSuccess = true;
    }else{
        isSuccess = false;
        value.emplace<std::monostate>();
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::StatSequence::StatSequence(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::StatSequence::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    statements.clear();
    Statement stmt(source, passes);
    if(stmt.parse()){
        statements.push_back(stmt);
        while(skipWhiteSpaces(source) && matchOne<';'>(source)){
            skipWhiteSpaces(source);
            if(stmt.parse()){
                statements.push_back(stmt);
            }
        }
        isSuccess = true;
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::IfStatement::IfStatement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    relation(source, passes), thenStat(source, passes)
{}
bool Parser::IfStatement::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    if(
        errorOnPartial(matchSequence<'i', 'f'>(source), 2,
            "expected 'if' in if statement"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && relation.parse(),
            "expected relation in if statement"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && (matchSequence<'t', 'h', 'e', 'n'>(source) == 4),
            "expected 'then' before if statement body"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && thenStat.parse(),
            "no if statement body"
        )
    ){
        if(skipWhiteSpaces(source) && errorOnPartial(matchSequence<'e', 'l', 's', 'e'>(source), 4,
            "expected 'else' in else statement"
        )){
            if(!errorOnFalse(
                skipWhiteSpaces(source) && elseStat.emplace(source, passes).parse(),
                "no else statement body"
            )){
                isSuccess = false;
                return runPassAfterParse(isSuccess, *this, passes);
            }
        }
        isSuccess = errorOnFalse(
            skipWhiteSpaces(source) && (matchSequence<'f', 'i'>(source) == 2),
            "expected 'fi' after if statement"
        );
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::WhileStatement::WhileStatement(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    relation(source, passes), statements(source, passes)
{}
bool Parser::WhileStatement::parse(){
    runPassBeforeParse(*this, passes);
    return runPassAfterParse((
        isSuccess = (
            errorOnPartial(matchSequence<'w', 'h', 'i', 'l', 'e'>(source), 5,
                "expected 'while' in while statement"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && relation.parse(),
                "expected relation operator in while statement"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && (matchSequence<'d', 'o'>(source) == 2),
                "expected 'do' before while statement body"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && statements.parse(),
                "no while statement body"
            )
            && errorOnFalse(
                skipWhiteSpaces(source)&& (matchSequence<'o', 'd'>(source) == 2),
                "expected 'od' after while statement body"
            )
        )),
    *this, passes);
}

Parser::TypeDecl::TypeDecl(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::TypeDecl::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    arraySizes.clear();
    unsigned int varMatch = matchSequence<'v', 'a', 'r'>(source);
    Number arrSize(source, passes);
    if(varMatch >= 2){
        if(errorOnFalse(varMatch == 3,
            "expected 'var' in type declaration"
        )){
            isSuccess = true;
            declType = Type::Variable;
        }
    }else if(
        errorOnPartial(matchSequence<'a', 'r', 'r', 'a', 'y'>(source), 5,
            "expected 'array' in type declaration"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && matchOne<'['>(source),
            "expected '[' in array type declaration"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && arrSize.parse(),
            "expected number in array type declaration"
        )
        && errorOnFalse(
            skipWhiteSpaces(source) && matchOne<']'>(source),
            "expected ']' in array type declaration"
        )
    ){
        arraySizes.push_back(arrSize);
        while(
            skipWhiteSpaces(source) && matchOne<'['>(source)
            && errorOnFalse(
                skipWhiteSpaces(source) && arrSize.parse(),
                "expected number in array type declaration"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && matchOne<']'>(source),
                "expected ']' in array type declaration"
            )
        ){
            arraySizes.push_back(arrSize);
        }
        isSuccess = true;
        declType = Type::Array;
        arraySizes.shrink_to_fit();
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::VarDecl::VarDecl(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    typeDecl(source, passes)
{}
bool Parser::VarDecl::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    identifiers.clear();
    Ident identifier(source, passes);
    if(
        typeDecl.parse()
        && errorOnFalse(
            skipWhiteSpaces(source) && identifier.parse(),
            "expected identifier in variable declaration"
        )
    ){
        identifiers.push_back(identifier);
        while(
            skipWhiteSpaces(source)
            && matchOne<','>(source)
            && errorOnFalse(
                skipWhiteSpaces(source) && identifier.parse(),
                "expected identifier in variable declaration"
            )
        ){
            identifiers.push_back(identifier);
        }
        identifiers.shrink_to_fit();
        isSuccess = errorOnFalse(
            skipWhiteSpaces(source) && matchOne<';'>(source),
            "expected ';' after variable declaration"
        );
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::FormalParam::FormalParam(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::FormalParam::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    identifiers.clear();
    if(errorOnFalse(
        matchOne<'('>(source) && skipWhiteSpaces(source),
        "expected '(' before formal parameters"
    )){
        Ident identifier(source, passes);
        if(identifier.parse()){
            identifiers.push_back(identifier);
            while(
                skipWhiteSpaces(source)
                && matchOne<','>(source)
                && errorOnFalse(
                    skipWhiteSpaces(source) && identifier.parse(),
                    "expected identifier after ',' in formal parameters"
                )
            ){
                identifiers.push_back(identifier);
            }
            identifiers.shrink_to_fit();
        }
        isSuccess = errorOnFalse(
            skipWhiteSpaces(source) && matchOne<')'>(source),
            "expected ')' after formal parameters"
        );
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::FuncBody::FuncBody(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes)
{}
bool Parser::FuncBody::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    varDecls.clear();
    VarDecl varDecl(source, passes);
    while(varDecl.parse() && skipWhiteSpaces(source)){
        varDecls.push_back(varDecl);
    }
    varDecls.shrink_to_fit();
    if(errorOnFalse(
        matchOne<'{'>(source) && skipWhiteSpaces(source),
        "expected '{' before function body"
    )){
        if(!statSequence.emplace(source, passes).parse()){
            statSequence.reset();
        };
        isSuccess = errorOnFalse(
            skipWhiteSpaces(source) && matchOne<'}'>(source),
            "expected '}' after function body"
        );
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

Parser::FuncDecl::FuncDecl(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    isVoid(false), identifier(source, passes), parameter(source, passes), body(source, passes)
{}
bool Parser::FuncDecl::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    isVoid = errorOnPartial(matchSequence<'v', 'o', 'i', 'd'>(source), 4,
        "expected 'void' in void function"
    );
    return runPassAfterParse((isSuccess = 
        (
            skipWhiteSpaces(source)
            && errorOnPartial(matchSequence<'f','u','n','c','t','i','o','n'>(source), 8,
                "expected 'function' in function declaration"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && identifier.parse(),
                "expected identifier in function signature"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && parameter.parse(),
                "expected formal parameter in function signature"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && matchOne<';'>(source),
                "expected ';' after function signature"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && body.parse(),
                "no function body"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && matchOne<';'>(source),
                "expected ';' after function declaration"
            )
        )),
        *this, passes
    );
}

Parser::Computation::Computation(Source& source, std::vector<std::reference_wrapper<Pass>>& passes): Interface(source, passes),
    statement(source, passes)
{}
bool Parser::Computation::parse(){
    runPassBeforeParse(*this, passes);
    isSuccess = false;
    varDecls.clear();
    funcDecls.clear();
    if(errorOnFalse(
        skipWhiteSpaces(source) && (matchSequence<'m', 'a', 'i', 'n'>(source) == 4),
        "expected 'main' in the begin of computation"
    )){
        VarDecl varDecl(source, passes);
        while(skipWhiteSpaces(source) && varDecl.parse()){
            varDecls.push_back(varDecl);
        }
        varDecls.shrink_to_fit();
        FuncDecl funcDecl(source, passes);
        while(skipWhiteSpaces(source) && funcDecl.parse()){
            funcDecls.push_back(funcDecl);
        }
        funcDecls.shrink_to_fit();
        isSuccess = 
            errorOnFalse(
                skipWhiteSpaces(source) && matchOne<'{'>(source), 
                "expected '{' before computation body"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && statement.parse(),
                "no statements in computation body"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && matchOne<'}'>(source),
                "expected '}' after computation body"
            )
            && errorOnFalse(
                skipWhiteSpaces(source) && matchOne<'.'>(source),
                "expected '.' in the end of computation"
            );
    }
    return runPassAfterParse(isSuccess, *this, passes);
}

void Parser::Pass::beforeParse(Letter&){}
void Parser::Pass::beforeParse(Digit&){}
void Parser::Pass::beforeParse(RelOp&){}
void Parser::Pass::beforeParse(Ident&){}
void Parser::Pass::beforeParse(Number&){}
void Parser::Pass::beforeParse(Designator&){}
void Parser::Pass::beforeParse(Factor&){}
void Parser::Pass::beforeParse(Term&){}
void Parser::Pass::beforeParse(Expression&){}
void Parser::Pass::beforeParse(Relation&){}
void Parser::Pass::beforeParse(Assignment&){}
void Parser::Pass::beforeParse(FuncCall&){}
void Parser::Pass::beforeParse(ReturnStatement&){}
void Parser::Pass::beforeParse(Statement&){}
void Parser::Pass::beforeParse(StatSequence&){}
void Parser::Pass::beforeParse(IfStatement&){}
void Parser::Pass::beforeParse(WhileStatement&){}
void Parser::Pass::beforeParse(TypeDecl&){}
void Parser::Pass::beforeParse(VarDecl&){}
void Parser::Pass::beforeParse(FormalParam&){}
void Parser::Pass::beforeParse(FuncBody&){}
void Parser::Pass::beforeParse(FuncDecl&){}
void Parser::Pass::beforeParse(Computation&){}

void Parser::Pass::afterParse(Letter&){}
void Parser::Pass::afterParse(Digit&){}
void Parser::Pass::afterParse(RelOp&){}
void Parser::Pass::afterParse(Ident&){}
void Parser::Pass::afterParse(Number&){}
void Parser::Pass::afterParse(Designator&){}
void Parser::Pass::afterParse(Factor&){}
void Parser::Pass::afterParse(Term&){}
void Parser::Pass::afterParse(Expression&){}
void Parser::Pass::afterParse(Relation&){}
void Parser::Pass::afterParse(Assignment&){}
void Parser::Pass::afterParse(FuncCall&){}
void Parser::Pass::afterParse(ReturnStatement&){}
void Parser::Pass::afterParse(Statement&){}
void Parser::Pass::afterParse(StatSequence&){}
void Parser::Pass::afterParse(IfStatement&){}
void Parser::Pass::afterParse(WhileStatement&){}
void Parser::Pass::afterParse(TypeDecl&){}
void Parser::Pass::afterParse(VarDecl&){}
void Parser::Pass::afterParse(FormalParam&){}
void Parser::Pass::afterParse(FuncBody&){}
void Parser::Pass::afterParse(FuncDecl&){}
void Parser::Pass::afterParse(Computation&){}