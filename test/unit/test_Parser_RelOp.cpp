#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, RelOp_equal){
    std::stringstream input("==");
    Source source(input);
    Parser::RelOp relOp(source);
    EXPECT_TRUE(relOp.parse());
}

TEST(unit_Parser, RelOp_not_equal){
    std::stringstream input("!=");
    Source source(input);
    Parser::RelOp relOp(source);
    EXPECT_TRUE(relOp.parse());
}

TEST(unit_Parser, RelOp_great){
    std::stringstream input(">");
    Source source(input);
    Parser::RelOp relOp(source);
    EXPECT_TRUE(relOp.parse());
}

TEST(unit_Parser, RelOp_great_equal){
    std::stringstream input(">=");
    Source source(input);
    Parser::RelOp relOp(source);
    EXPECT_TRUE(relOp.parse());
}

TEST(unit_Parser, RelOp_less){
    std::stringstream input("<");
    Source source(input);
    Parser::RelOp relOp(source);
    EXPECT_TRUE(relOp.parse());
}

TEST(unit_Parser, RelOp_less_equal){
    std::stringstream input("<=");
    Source source(input);
    Parser::RelOp relOp(source);
    EXPECT_TRUE(relOp.parse());
}

TEST(unit_Parser, RelOp_unknown){
    std::stringstream input("f=");
    Source source(input);
    Parser::RelOp relOp(source);
    EXPECT_FALSE(relOp.parse());
    EXPECT_EQ(source.get(), 'f');
    EXPECT_EQ(source.get(), '=');
    input.clear();
    input.str("=#");
    EXPECT_FALSE(relOp.parse());
    EXPECT_EQ(source.get(), '=');
    EXPECT_EQ(source.get(), '#');
}