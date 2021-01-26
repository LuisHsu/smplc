#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Computation_no_varDecl_nor_funcDecl){
    std::stringstream input("main {call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_TRUE(comp.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Computation_one_varDecl_one_funcDecl){
    std::stringstream input("main var x; function add(); {}; {call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_TRUE(comp.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Computation_one_varDecl){
    std::stringstream input("main var x; {call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_TRUE(comp.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Computation_two_varDecl){
    std::stringstream input("main var x; var y;{call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_TRUE(comp.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Computation_one_varDecl_two_funcDecl){
    std::stringstream input("main var x; function add(); {}; function sub(); {}; {call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_TRUE(comp.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Computation_two_varDecl_one_funcDecl){
    std::stringstream input("main var x; var y; function add(); {}; {call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_TRUE(comp.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Computation_one_funcDecl){
    std::stringstream input("main function add(); {}; {call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_TRUE(comp.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Computation_two_funcDecl){
    std::stringstream input("main function add(); {}; function sub(); {}; {call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_TRUE(comp.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Computation_no_trailing_dot){
    std::stringstream input("main {call OutputNewLine}");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_FALSE(comp.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Computation_no_main_keyword){
    std::stringstream input("{call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_FALSE(comp.parse());
    EXPECT_EQ(source.get(), '{');
}

TEST(unit_Parser, Computation_no_left_brace){
    std::stringstream input("main call OutputNewLine}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_FALSE(comp.parse());
    EXPECT_EQ(source.get(), 'c');
}

TEST(unit_Parser, Computation_no_right_brace){
    std::stringstream input("main {call OutputNewLine .");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_FALSE(comp.parse());
    EXPECT_EQ(source.get(), '.');
}

TEST(unit_Parser, Computation_no_statSequence){
    std::stringstream input("main {}.");
    Source source(input);
    Parser::Computation comp(source);
    EXPECT_FALSE(comp.parse());
    EXPECT_EQ(source.get(), '}');
}