#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Letter_a_to_z){
    std::stringstream input("abcdefghijklmnopqrstuvwxyz");
    Source source(input);
    Parser::Letter letter(source);
    for(char i = 'a'; i <= 'z'; ++i){
        EXPECT_TRUE(letter.parse());
    }
    EXPECT_FALSE(letter.parse());
}

TEST(unit_Parser, Letter_unknown){
    std::stringstream input("@$");
    Source source(input);
    Parser::Letter letter(source);
    EXPECT_FALSE(letter.parse());
    EXPECT_EQ(source.get(), '@');
    EXPECT_FALSE(letter.parse());
    EXPECT_EQ(source.get(), '$');
}