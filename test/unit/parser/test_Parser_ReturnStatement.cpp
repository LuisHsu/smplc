#include <gtest/gtest.h>

#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, ReturnStatement_with_expression){
    std::stringstream input("return 0");
    Source source(input);
    Parser::ReturnStatement stmt(source);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, ReturnStatement_without_expression){
    std::stringstream input("return");
    Source source(input);
    Parser::ReturnStatement stmt(source);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}
