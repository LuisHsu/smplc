#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Digit_0_to_9){
    std::stringstream input("1234567890");
    Source source(input);
    Parser::Digit digit(source);
    for(char i = '0'; i <= '9'; ++i){
        EXPECT_TRUE(digit.parse());
    }
    EXPECT_FALSE(digit.parse());
}

TEST(unit_Parser, Digit_unknown){
    std::stringstream input("a^&");
    Source source(input);
    Parser::Digit digit(source);
    EXPECT_FALSE(digit.parse());
    EXPECT_FALSE(digit.parse());
    EXPECT_FALSE(digit.parse());
}