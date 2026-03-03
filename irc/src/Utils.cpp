#include "Utils.hpp"

#include <cctype>
#include <sstream>

std::vector<std::string> split(const std::string &str, char delim)
{
    std::vector<std::string> elems;
    std::string current;
    for (std::string::size_type i = 0; i < str.size(); ++i)
    {
        if (str[i] == delim)
        {
            elems.push_back(current);
            current.clear();
        }
        else
            current += str[i];
    }
    elems.push_back(current);
    return elems;
}

std::string trim(const std::string &str)
{
    std::string::size_type start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start])))
        ++start;
    std::string::size_type end = str.size();
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
        --end;
    return str.substr(start, end - start);
}

std::string toUpper(const std::string &str)
{
    std::string res;
    for (std::string::size_type i = 0; i < str.size(); ++i)
        res += static_cast<char>(std::toupper(static_cast<unsigned char>(str[i])));
    return res;
}

std::string itoa(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

bool isValidNickname(const std::string &nick)
{
    if (nick.empty() || nick.size() > 9)
        return false;
    char first = nick[0];
    if (!(std::isalpha(static_cast<unsigned char>(first)) || first == '_' || first == '-'))
        return false;
    for (std::string::size_type i = 1; i < nick.size(); ++i)
    {
        char c = nick[i];
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-'))
            return false;
    }
    return true;
}

bool isChannelName(const std::string &name)
{
    return !name.empty() && name[0] == '#' && name.size() > 1;
}
