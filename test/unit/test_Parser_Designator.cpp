#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Designator_ident){
    std::stringstream input("id");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_TRUE(designator.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Designator_array){
    std::stringstream input("id[3]");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_TRUE(designator.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Designator_2d_array){
    std::stringstream input("id[4][3]");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_TRUE(designator.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Designator_missing_left_bracket){
    std::stringstream input("id3]");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_TRUE(designator.parse());
    EXPECT_EQ(source.get(), ']');
}

TEST(unit_Parser, Designator_missing_right_bracket){
    std::stringstream input("id[3");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_FALSE(designator.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Designator_missing_expression){
    std::stringstream input("id[]");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_FALSE(designator.parse());
    EXPECT_EQ(source.get(), ']');
}

TEST(unit_Parser, Designator_invalid){
    std::stringstream input("25=");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_FALSE(designator.parse());
    EXPECT_EQ(source.get(), '2');
}