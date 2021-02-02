#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, FuncCall_ident_only){
    std::stringstream input("call testfunc");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::FuncCall func(source, passes);
    EXPECT_TRUE(func.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncCall_miss_call){
    std::stringstream input("testfunc()");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::FuncCall func(source, passes);
    EXPECT_FALSE(func.parse());
    EXPECT_EQ(source.get(), 't');
}

TEST(unit_Parser, FuncCall_ident_with_parenthesis){
    std::stringstream input("call testfunc()");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::FuncCall func(source, passes);
    EXPECT_TRUE(func.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncCall_ident_miss_left_parenthesis){
    std::stringstream input("call testfunc)");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::FuncCall func(source, passes);
    EXPECT_TRUE(func.parse());
    EXPECT_EQ(source.get(), ')');
}

TEST(unit_Parser, FuncCall_ident_miss_right_parenthesis){
    std::stringstream input("call testfunc(");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::FuncCall func(source, passes);
    EXPECT_FALSE(func.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncCall_ident_with_param){
    std::stringstream input("call testfunc(123)");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::FuncCall func(source, passes);
    EXPECT_TRUE(func.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncCall_ident_with_more_param){
    std::stringstream input("call testfunc(123, 456)");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::FuncCall func(source, passes);
    EXPECT_TRUE(func.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncCall_ident_miss_one_param){
    std::stringstream input("call testfunc(123,)");
    Source source(input);
    std::vector<Parser::Pass> passes;
    Parser::FuncCall func(source, passes);
    EXPECT_FALSE(func.parse());
    EXPECT_EQ(source.get(), ')');
}