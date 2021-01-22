#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Designator_ident){
    std::stringstream input("id");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_TRUE(designator.parse());
}

TEST(unit_Parser, DISABLED_Designator_array){
    std::stringstream input("id[3]");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_TRUE(designator.parse());
}

TEST(unit_Parser, DISABLED_Designator_2d_array){
    std::stringstream input("id[4][3]");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_TRUE(designator.parse());
}

TEST(unit_Parser, DISABLED_Designator_missing_left_bracket){
    std::stringstream input("id3]");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_FALSE(designator.parse());
}

TEST(unit_Parser, DISABLED_Designator_missing_right_bracket){
    std::stringstream input("id[3");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_FALSE(designator.parse());
}

TEST(unit_Parser, DISABLED_Designator_missing_expression){
    std::stringstream input("id[]");
    Source source(input);
    Parser::Designator designator(source);
    EXPECT_FALSE(designator.parse());
}