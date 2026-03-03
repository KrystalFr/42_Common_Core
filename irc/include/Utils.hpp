#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

std::vector<std::string> split(const std::string &str, char delim);
std::string trim(const std::string &str);
std::string toUpper(const std::string &str);
std::string itoa(int value);
bool isValidNickname(const std::string &nick);
bool isChannelName(const std::string &name);

#endif
