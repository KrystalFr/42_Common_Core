#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

class Logger
{
public:
    static void info(const std::string &msg);
    static void warn(const std::string &msg);
    static void error(const std::string &msg);
    static void debug(const std::string &msg);
    static void setDebug(bool enabled);

private:
    static bool _debugEnabled;
    static void print(const std::string &label, const std::string &color, const std::string &msg);
};

#endif
