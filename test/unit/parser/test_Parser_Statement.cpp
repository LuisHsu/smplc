#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Statement_assignment){
    std::stringstream input("let testid <- 4");
    Source source(input);
    Parser::Statement stmt(source);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Statement_funcCall){
    std::stringstream input("call testFunc");
    Source source(input);
    Parser::Statement stmt(source);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Statement_ifStatement){
    std::stringstream input("if val > 2 then call testfunc fi");
    Source source(input);
    Parser::Statement stmt(source);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Statement_whileStatement){
    std::stringstream input("while i < 5 do call OutputNewLine od");
    Source source(input);
    Parser::Statement stmt(source);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Statement_returnStatement){
    std::stringstream input("return 3");
    Source source(input);
    Parser::Statement stmt(source);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}