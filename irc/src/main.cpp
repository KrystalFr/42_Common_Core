#include "Server.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>

static int toInt(const std::string &s)
{
    std::istringstream iss(s);
    int value = 0;
    if (!(iss >> value))
        return -1;
    return value;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    int port = toInt(argv[1]);
    if (port < 6667 || port > 6669)
    {
        std::cerr << "Invalid port" << std::endl;
        return 1;
    }
    std::string password = argv[2];
    try
    {
        Server server(port, password);
        Logger::info("Starting server on port " + itoa(port));
        server.run();
    }
    catch (const std::exception &e)
    {
        Logger::error(std::string("Error: ") + e.what());
        return 1;
    }
    return 0;
}
