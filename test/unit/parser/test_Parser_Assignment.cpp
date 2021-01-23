#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Assignment_valid){
    std::stringstream input("let testvar <- 4");
    Source source(input);
    Parser::Assignment assignment(source);
    EXPECT_TRUE(assignment.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Assignment_no_let){
    std::stringstream input("testvar <- 4");
    Source source(input);
    Parser::Assignment assignment(source);
    EXPECT_FALSE(assignment.parse());
    EXPECT_EQ(source.get(), 't');
}

TEST(unit_Parser, Assignment_no_designator){
    std::stringstream input("let <- 4");
    Source source(input);
    Parser::Assignment assignment(source);
    EXPECT_FALSE(assignment.parse());
    EXPECT_EQ(source.get(), '<');
}

TEST(unit_Parser, Assignment_no_arrow){
    std::stringstream input("let testvar 4");
    Source source(input);
    Parser::Assignment assignment(source);
    EXPECT_FALSE(assignment.parse());
    EXPECT_EQ(source.get(), '4');
}

TEST(unit_Parser, Assignment_no_expression){
    std::stringstream input("let testvar <-");
    Source source(input);
    Parser::Assignment assignment(source);
    EXPECT_FALSE(assignment.parse());
    EXPECT_EQ(source.get(), -1);
}