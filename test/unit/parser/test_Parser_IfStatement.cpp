#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, IfStatement_without_else){
    std::stringstream input("if 3 > 0 then let testid <- 4 fi");
    Source source(input);
    Parser::IfStatement stmt(source);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, IfStatement_with_else){
    std::stringstream input("if 3 > 0 then let testid <- 4 else let testid <- 5 fi");
    Source source(input);
    Parser::IfStatement stmt(source);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, IfStatement_without_fi){
    std::stringstream input("if 3 > 0 then let testid <- 4");
    Source source(input);
    Parser::IfStatement stmt(source);
    EXPECT_FALSE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, IfStatement_without_then){
    std::stringstream input("if 3 > 0 let testid <- 4 fi");
    Source source(input);
    Parser::IfStatement stmt(source);
    EXPECT_FALSE(stmt.parse());
    EXPECT_EQ(source.get(), 'l');
}

TEST(unit_Parser, IfStatement_without_relation){
    std::stringstream input("if then let testid <- 4 fi");
    Source source(input);
    Parser::IfStatement stmt(source);
    EXPECT_FALSE(stmt.parse());
    EXPECT_EQ(source.get(), 'l');
}