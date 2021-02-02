#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, WhileStatement_valid){
    std::stringstream input("while testid > 4 do call OutputNum(x); od");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::WhileStatement stmt(source, passes);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, WhileStatement_no_od){
    std::stringstream input("while testid > 4 do call OutputNum(x);");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::WhileStatement stmt(source, passes);
    EXPECT_FALSE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, WhileStatement_no_statement){
    std::stringstream input("while testid > 4 do od");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::WhileStatement stmt(source, passes);
    EXPECT_FALSE(stmt.parse());
    EXPECT_EQ(source.get(), 'o');
}

TEST(unit_Parser, WhileStatement_no_do){
    std::stringstream input("while testid > 4 call OutputNum(x); od");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::WhileStatement stmt(source, passes);
    EXPECT_FALSE(stmt.parse());
    EXPECT_EQ(source.get(), 'c');
}

TEST(unit_Parser, WhileStatement_no_relation){
    std::stringstream input("while do call OutputNum(x); od");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::WhileStatement stmt(source, passes);
    EXPECT_FALSE(stmt.parse());
    EXPECT_EQ(source.get(), 'c');
}

TEST(unit_Parser, WhileStatement_no_while){
    std::stringstream input("testid > 4 do call OutputNum(x); od");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::WhileStatement stmt(source, passes);
    EXPECT_FALSE(stmt.parse());
    EXPECT_EQ(source.get(), 't');
}