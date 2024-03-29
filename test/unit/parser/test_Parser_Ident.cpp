#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Ident_letter_only){
    std::stringstream input("abcd 8");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::Ident ident(source, passes);
    EXPECT_TRUE(ident.parse());
    EXPECT_EQ(source.get(), ' ');
    EXPECT_EQ(source.get(), '8');
}

TEST(unit_Parser, Ident_letter_digit){
    std::stringstream input("ab23 ");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::Ident ident(source, passes);
    EXPECT_TRUE(ident.parse());
    EXPECT_EQ(source.get(), ' ');
}

TEST(unit_Parser, Ident_digit_begin){
    std::stringstream input("23as");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::Ident ident(source, passes);
    EXPECT_FALSE(ident.parse());
    EXPECT_EQ(source.get(), '2');
}

TEST(unit_Parser, Ident_invalid){
    std::stringstream input("=ab");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::Ident ident(source, passes);
    EXPECT_FALSE(ident.parse());
    EXPECT_EQ(source.get(), '=');
    EXPECT_EQ(source.get(), 'a');
    EXPECT_EQ(source.get(), 'b');
}