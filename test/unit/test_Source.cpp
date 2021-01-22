#include <gtest/gtest.h>

#include <sstream>
#include <Exception.hpp>
#include <Source.hpp>

TEST(unit_Source, white_space){
    std::stringstream input(" ");
    EXPECT_EQ(Source(input).get(), ' ');
    input.clear();
    input.str("\v");
    EXPECT_EQ(Source(input).get(), ' ');
    input.clear();
    input.str("\r");
    EXPECT_EQ(Source(input).get(), ' ');
    input.clear();
    input.str("\t");
    EXPECT_EQ(Source(input).get(), ' ');
    input.clear();
    input.str("\n");
    EXPECT_EQ(Source(input).get(), ' ');
}

TEST(unit_Source, consecutive_white_space){
    std::stringstream input("  ");
    Source source1(input);
    EXPECT_EQ(source1.get(), ' ');
    EXPECT_EQ(source1.get(), -1);
    input.clear();
    input.str(" \v");
    Source source2(input);
    EXPECT_EQ(source2.get(), ' ');
    EXPECT_EQ(source2.get(), -1);
    input.clear();
    input.str("\t ");
    Source source3(input);
    EXPECT_EQ(source3.get(), ' ');
    EXPECT_EQ(source3.get(), -1);
    input.clear();
    input.str("\n\n");
    Source source4(input);
    EXPECT_EQ(source4.get(), ' ');
    EXPECT_EQ(source4.get(), -1);
}

TEST(unit_Source, single_line_comment){
    std::stringstream input(" // test\n");
    Source source1(input);
    EXPECT_EQ(source1.get(), ' ');
    EXPECT_EQ(source1.get(), -1);
    input.clear();
    input.str(" // test\n i //test2\n");
    Source source2(input);
    EXPECT_EQ(source2.get(), ' ');
    EXPECT_EQ(source2.get(), 'i');
    EXPECT_EQ(source2.get(), ' ');
    EXPECT_EQ(source2.get(), -1);
}

TEST(unit_Source, multi_line_comment){
    std::stringstream input(" /* test */\n");
    Source source1(input);
    EXPECT_EQ(source1.get(), ' ');
    EXPECT_EQ(source1.get(), -1);

    input.clear();
    input.str("/**/");
    Source source2(input);
    EXPECT_EQ(source2.get(), ' ');
    EXPECT_EQ(source2.get(), -1);

    input.clear();
    input.str("/* /* */ */");
    Source source3(input);
    EXPECT_EQ(source3.get(), ' ');
    EXPECT_EQ(source3.get(), -1);

    input.clear();
    input.str("/* /* */");
    Source source4(input);
    EXPECT_THROW(source4.get(), Exception);
}
