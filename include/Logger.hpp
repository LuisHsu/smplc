#ifndef SMPLC_Logger_DEF
#define SMPLC_Logger_DEF

#include <istream>
#include <optional>
#include <string>
#include <queue>
#include <functional>

enum class LogLevel{
    Info,
    Warning,
    Error,
};

class Logger{
private:
    Logger();
    static Logger logger;
    unsigned int numInfo, numWarning, numError, lineNum;
    std::queue<std::tuple<unsigned int, LogLevel, std::string>> logs;
public:
    static void put(LogLevel level, std::string msg);
    static unsigned int infoCount();
    static unsigned int warningCount();
    static unsigned int errorCount();
    static void increaseLine();
    static void dump(std::function<void(unsigned int, LogLevel, std::string)> callback);
};

#endif