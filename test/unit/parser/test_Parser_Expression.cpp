#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Expression_term_only){
    std::stringstream input("test1");
    Source source(input);
    Parser::Expression expression(source);
    EXPECT_TRUE(expression.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Expression_term_plus){
    std::stringstream input("test1 + test2");
    Source source(input);
    Parser::Expression expression(source);
    EXPECT_TRUE(expression.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Expression_term_minus){
    std::stringstream input("test1 - test2");
    Source source(input);
    Parser::Expression expression(source);
    EXPECT_TRUE(expression.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Expression_more_term){
    std::stringstream input("test1 + test2 - test3");
    Source source(input);
    Parser::Expression expression(source);
    EXPECT_TRUE(expression.parse());
    EXPECT_EQ(source.get(), -1);
}
