#include <gtest/gtest.h>

#include <string>
#include <Logger.hpp>

TEST(unit_Logger, put_info){
    Logger::put(LogLevel::Info, "testInfo");
    EXPECT_EQ(Logger::infoCount(), 1);
    Logger::put(LogLevel::Info, "testInfo2");
    EXPECT_EQ(Logger::infoCount(), 2);
}

TEST(unit_Logger, put_warning){
    Logger::put(LogLevel::Warning, "testWarn");
    EXPECT_EQ(Logger::warningCount(), 1);
    Logger::put(LogLevel::Warning, "testWarn2");
    EXPECT_EQ(Logger::warningCount(), 2);
}

TEST(unit_Logger, put_error){
    Logger::put(LogLevel::Error, "testErr");
    EXPECT_EQ(Logger::errorCount(), 1);
    Logger::put(LogLevel::Error, "testErr2");
    EXPECT_EQ(Logger::errorCount(), 2);
}

TEST(unit_Logger, dump){
    Logger::put(LogLevel::Error, "testErr");
    Logger::put(LogLevel::Warning, "testWarn");
    Logger::put(LogLevel::Info, "testInfo");
    unsigned int count = 0;
    Logger::dump([&count](unsigned int line, LogLevel level, std::string msg) -> void {
        switch (count++){
        case 0:
            EXPECT_EQ(level, LogLevel::Error);
            EXPECT_STREQ(msg.c_str(), "testErr");
            break;
        case 1:
            EXPECT_EQ(level, LogLevel::Warning);
            EXPECT_STREQ(msg.c_str(), "testWarn");
            break;
        case 2:
            EXPECT_EQ(level, LogLevel::Info);
            EXPECT_STREQ(msg.c_str(), "testInfo");
            break;
        }
    });
}