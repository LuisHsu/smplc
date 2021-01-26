#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, FuncBody_empty){
    std::stringstream input("{}");
    Source source(input);
    Parser::FuncBody body(source);
    EXPECT_TRUE(body.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncBody_with_statSequence){
    std::stringstream input("{call testFunc;}");
    Source source(input);
    Parser::FuncBody body(source);
    EXPECT_TRUE(body.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncBody_with_one_varDecl){
    std::stringstream input("var x; {call testFunc;}");
    Source source(input);
    Parser::FuncBody body(source);
    EXPECT_TRUE(body.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncBody_with_two_varDecl){
    std::stringstream input("var x; var y; {call testFunc;}");
    Source source(input);
    Parser::FuncBody body(source);
    EXPECT_TRUE(body.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, FuncBody_no_left_brace){
    std::stringstream input("call testFunc;}");
    Source source(input);
    Parser::FuncBody body(source);
    EXPECT_FALSE(body.parse());
    EXPECT_EQ(source.get(), 'c');
}

TEST(unit_Parser, FuncBody_no_right_brace){
    std::stringstream input("{call testFunc;");
    Source source(input);
    Parser::FuncBody body(source);
    EXPECT_FALSE(body.parse());
    EXPECT_EQ(source.get(), -1);
}