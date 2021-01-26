#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, FormalParam_no_ident){
    std::stringstream input("()");
    Source source(input);
    Parser::FormalParam param(source);
    EXPECT_TRUE(param.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FormalParam_one_ident){
    std::stringstream input("(x1)");
    Source source(input);
    Parser::FormalParam param(source);
    EXPECT_TRUE(param.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FormalParam_two_idents){
    std::stringstream input("(testid, test2)");
    Source source(input);
    Parser::FormalParam param(source);
    EXPECT_TRUE(param.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FormalParam_no_left_parenthesis){
    std::stringstream input("testid, test2)");
    Source source(input);
    Parser::FormalParam param(source);
    EXPECT_FALSE(param.parse());
    EXPECT_EQ(source.get(), 't');
}

TEST(unit_Parser, FormalParam_no_right_parenthesis){
    std::stringstream input("(x1");
    Source source(input);
    Parser::FormalParam param(source);
    EXPECT_FALSE(param.parse());
    EXPECT_EQ(source.get(), -1);
}
