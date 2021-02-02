#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, FuncDecl_non_void){
    std::stringstream input("function testFunc(); { return 4; };");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::FuncDecl decl(source, passes);
    EXPECT_TRUE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncDecl_void){
    std::stringstream input("void function testFunc(); {};");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::FuncDecl decl(source, passes);
    EXPECT_TRUE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncDecl_no_traling_semicolon){
    std::stringstream input("void function testFunc(); {}");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::FuncDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncDecl_no_function_body){
    std::stringstream input("void function testFunc();;");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::FuncDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), ';');
}

TEST(unit_Parser, FuncDecl_no_middle_semicolon){
    std::stringstream input("void function testFunc() {};");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::FuncDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), '{');
}

TEST(unit_Parser, FuncDecl_no_formal_param){
    std::stringstream input("void function testFunc; {};");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::FuncDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), ';');
}

TEST(unit_Parser, FuncDecl_no_ident){
    std::stringstream input("void function (); {};");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::FuncDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), '(');
}

TEST(unit_Parser, FuncDecl_no_function_keyword){
    std::stringstream input("testFunc(); {};");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::FuncDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), 't');
}