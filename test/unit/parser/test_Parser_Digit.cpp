#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Digit_0_to_9){
    std::stringstream input("1234567890");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Digit digit(source, passes);
    for(char i = '0'; i <= '9'; ++i){
        EXPECT_TRUE(digit.parse());
    }
    EXPECT_FALSE(digit.parse());
}

TEST(unit_Parser, Digit_unknown){
    std::stringstream input("a^2&");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Digit digit(source, passes);
    EXPECT_FALSE(digit.parse());
    EXPECT_EQ(source.get(), 'a');
    EXPECT_FALSE(digit.parse());
    EXPECT_EQ(source.get(), '^');
    EXPECT_TRUE(digit.parse());
    EXPECT_FALSE(digit.parse());
    EXPECT_EQ(source.get(), '&');
}