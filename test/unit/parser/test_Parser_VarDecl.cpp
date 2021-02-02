#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, VarDecl_one_ident){
    std::stringstream input("var testid;");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::VarDecl decl(source, passes);
    EXPECT_TRUE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, VarDecl_two_idents){
    std::stringstream input("var testid, test2;");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::VarDecl decl(source, passes);
    EXPECT_TRUE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, VarDecl_no_semicolon){
    std::stringstream input("var testid, test2");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::VarDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, VarDecl_no_ident){
    std::stringstream input("var ;");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::VarDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), ';');
}

TEST(unit_Parser, VarDecl_no_typeDecl){
    std::stringstream input("ident;");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::VarDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), 'i');
}