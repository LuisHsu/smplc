#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Factor_designator){
    std::stringstream input("testid");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Factor factor(source, passes);
    EXPECT_TRUE(factor.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Factor_number){
    std::stringstream input("456");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Factor factor(source, passes);
    EXPECT_TRUE(factor.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Factor_invalid){
    std::stringstream input("=#456");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Factor factor(source, passes);
    EXPECT_FALSE(factor.parse());
    EXPECT_EQ(source.get(), '=');
}

TEST(unit_Parser, Factor_expression){
    std::stringstream input("(456)");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Factor factor(source, passes);
    EXPECT_TRUE(factor.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Factor_funcCall){
    std::stringstream input("call test");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::Factor factor(source, passes);
    EXPECT_TRUE(factor.parse());
    EXPECT_EQ(source.get(), -1);
}