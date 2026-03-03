#include "Logger.hpp"

#include <iostream>

bool Logger::_debugEnabled = true;

static const char *CLR_RESET = "\033[0m";
static const char *CLR_INFO = "\033[32m";
static const char *CLR_WARN = "\033[33m";
static const char *CLR_ERR = "\033[31m";
static const char *CLR_DBG = "\033[36m";

void Logger::print(const std::string &label, const std::string &color, const std::string &msg)
{
    std::cout << color << "[" << label << "] " << msg << CLR_RESET << std::endl;
}

void Logger::info(const std::string &msg)
{
    print("INFO", CLR_INFO, msg);
}

void Logger::warn(const std::string &msg)
{
    print("WARN", CLR_WARN, msg);
}

void Logger::error(const std::string &msg)
{
    print("ERROR", CLR_ERR, msg);
}

void Logger::debug(const std::string &msg)
{
    if (_debugEnabled)
        print("DEBUG", CLR_DBG, msg);
}

void Logger::setDebug(bool enabled)
{
    _debugEnabled = enabled;
}
