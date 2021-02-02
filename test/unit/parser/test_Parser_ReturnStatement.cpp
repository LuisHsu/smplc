#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, ReturnStatement_with_expression){
    std::stringstream input("return 0");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::ReturnStatement stmt(source, passes);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, ReturnStatement_without_expression){
    std::stringstream input("return");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::ReturnStatement stmt(source, passes);
    EXPECT_TRUE(stmt.parse());
    EXPECT_EQ(source.get(), -1);
}
