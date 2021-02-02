#include <gtest/gtest.h>

#include <vector>
#include <sstream>
#include <Source.hpp>
#include <Parser.hpp>

TEST(unit_Parser, Term_factor_only){
    std::stringstream input("test1");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::Term term(source, passes);
    EXPECT_TRUE(term.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Term_factor_times){
    std::stringstream input("test1 * test2");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::Term term(source, passes);
    EXPECT_TRUE(term.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Term_factor_divide){
    std::stringstream input("test1 / test2");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::Term term(source, passes);
    EXPECT_TRUE(term.parse());
    EXPECT_EQ(source.get(), -1);
}

TEST(unit_Parser, Term_more_factor){
    std::stringstream input("test1 * test2 * test3");
    Source source(input);
    std::vector<std::reference_wrapper<Parser::Pass>> passes;
    Parser::Term term(source, passes);
    EXPECT_TRUE(term.parse());
    EXPECT_EQ(source.get(), -1);
}
