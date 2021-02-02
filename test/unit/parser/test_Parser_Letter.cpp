#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Letter_a_to_z){
    std::stringstream input("abcdefghijklmnopqrstuvwxyz");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Letter letter(source, passes);
    for(char i = 'a'; i <= 'z'; ++i){
        EXPECT_TRUE(letter.parse());
    }
    EXPECT_FALSE(letter.parse());
}

TEST(unit_Parser, Letter_A_to_Z){
    std::stringstream input("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Letter letter(source, passes);
    for(char i = 'A'; i <= 'Z'; ++i){
        EXPECT_TRUE(letter.parse());
    }
    EXPECT_FALSE(letter.parse());
}

TEST(unit_Parser, Letter_unknown){
    std::stringstream input("@$");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Letter letter(source, passes);
    EXPECT_FALSE(letter.parse());
    EXPECT_EQ(source.get(), '@');
    EXPECT_FALSE(letter.parse());
    EXPECT_EQ(source.get(), '$');
}