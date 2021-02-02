#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, StatSequence_one_statement){
    std::stringstream input("let testid <- 4 ;");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::StatSequence stmt(source, passes);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, StatSequence_without_semicolon){
    std::stringstream input("call testFunc");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::StatSequence stmt(source, passes);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, StatSequence_two_statements){
    std::stringstream input("let testid1 <- 4 ; let testid2 <- 3;");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::StatSequence stmt(source, passes);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, StatSequence_two_statements_no_separate){
    std::stringstream input("let testid1 <- 4 let testid2 <- 3;");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::StatSequence stmt(source, passes);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), 'l');
}