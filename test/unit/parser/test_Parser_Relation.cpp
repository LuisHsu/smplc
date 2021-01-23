#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Relation_expression_expression){
    std::stringstream input("testid[2] > testid1");
    Source source(input);
    Parser::Relation relation(source);
    EXPECT_TRUE(relation.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Relation_no_relop){
    std::stringstream input("testid[2]");
    Source source(input);
    Parser::Relation relation(source);
    EXPECT_FALSE(relation.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Relation_no_second_expression){
    std::stringstream input("testid[2] <");
    Source source(input);
    Parser::Relation relation(source);
    EXPECT_FALSE(relation.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Relation_no_first_expression){
    std::stringstream input(">= testid[2]");
    Source source(input);
    Parser::Relation relation(source);
    EXPECT_FALSE(relation.parse());
    EXPECT_EQ(source.get(), '>');
}