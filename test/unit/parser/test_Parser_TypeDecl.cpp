#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, TypeDecl_var){
    std::stringstream input("var");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::TypeDecl decl(source, passes);
    EXPECT_TRUE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, TypeDecl_array_1d){
    std::stringstream input("array[4]");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::TypeDecl decl(source, passes);
    EXPECT_TRUE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, TypeDecl_array_2d){
    std::stringstream input("array[7][9]");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::TypeDecl decl(source, passes);
    EXPECT_TRUE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, TypeDecl_array_no_number_and_brackets){
    std::stringstream input("array");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::TypeDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, TypeDecl_array_no_array){
    std::stringstream input("[7][9]");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::TypeDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), '[');
}

TEST(unit_Parser, TypeDecl_array_no_left_bracket){
    std::stringstream input("array4]");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::TypeDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), '4');
}

TEST(unit_Parser, TypeDecl_array_no_right_bracket){
    std::stringstream input("array[4");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::TypeDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, TypeDecl_array_no_number){
    std::stringstream input("array[]");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::TypeDecl decl(source, passes);
    EXPECT_FALSE(decl.parse());
    EXPECT_EQ(source.get(), ']');
}