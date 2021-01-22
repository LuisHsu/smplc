#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Number_digit_only){
    std::stringstream input("7493");
    Source source(input);
    Parser::Number number(source);
    EXPECT_TRUE(number.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Number_letter_after_digit){
    std::stringstream input("23a");
    Source source(input);
    Parser::Number number(source);
    EXPECT_TRUE(number.parse());
    EXPECT_EQ(source.get(), 'a');
}

TEST(unit_Parser, Number_letter_begin){
    std::stringstream input("as23");
    Source source(input);
    Parser::Number number(source);
    EXPECT_FALSE(number.parse());
    EXPECT_EQ(source.get(), 'a');
}

TEST(unit_Parser, Number_invalid){
    std::stringstream input("=10");
    Source source(input);
    Parser::Number number(source);
    EXPECT_FALSE(number.parse());
    EXPECT_EQ(source.get(), '=');
    EXPECT_EQ(source.get(), '1');
    EXPECT_EQ(source.get(), '0');
}