#include "Server.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

static const int BUFFER_SIZE = 4096;
static volatile sig_atomic_t g_stopRequested = 0;

static void handleSignal(int signum)
{
    if (signum == SIGINT || signum == SIGTERM || signum == SIGQUIT || signum == SIGTSTP)
        g_stopRequested = 1;
}

Server::Server(int port, const std::string &password) : _port(port), _password(password), _listenFd(-1), _running(false)
{
    setupSignalHandlers();
    _listenFd = setupListener();
    struct pollfd pfd;
    pfd.fd = _listenFd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollFds.push_back(pfd);
    Logger::info("Listening socket ready");
}

Server::~Server()
{
    stop();
}


int Server::setupListener()
{
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string portStr = itoa(_port);
    struct addrinfo *res = 0;
    if (getaddrinfo(0, portStr.c_str(), &hints, &res) != 0)
        throw std::runtime_error("getaddrinfo failed");

    int listenFd = -1;
    for (struct addrinfo *p = res; p; p = p->ai_next)
    {
        listenFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listenFd < 0)
            continue;
        int opt = 1;
        setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (fcntl(listenFd, F_SETFL, O_NONBLOCK) < 0)
        {
            close(listenFd);
            listenFd = -1;
            continue;
        }
        if (bind(listenFd, p->ai_addr, p->ai_addrlen) == 0)
        {
            if (listen(listenFd, 10) == 0)
                break;
        }
        close(listenFd);
        listenFd = -1;

    }
    freeaddrinfo(res);
    if (listenFd < 0)
        throw std::runtime_error("Unable to set up listening socket");
    return listenFd;
}

void Server::setupSignalHandlers()
{
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGTERM, &sa, 0);
    sigaction(SIGQUIT, &sa, 0);
    sigaction(SIGTSTP, &sa, 0);
}

void Server::run()
{
    _running = true;
    mainLoop();
}

void Server::stop()
{
    if (_listenFd >= 0)
        close(_listenFd);
    _listenFd = -1;
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        close(it->first);
    _clients.clear();
    _pollFds.clear();
    _running = false;
}

void Server::mainLoop()
{
    while (_running)
    {
        if (g_stopRequested)
        {
            _running = false;
            break;
        }
        time_t now = time(0);
        for (size_t i = 1; i < _pollFds.size(); ++i)
        {
            std::map<int, Client>::iterator it = _clients.find(_pollFds[i].fd);
            if (it != _clients.end() && it->second.hasPendingSend())
                _pollFds[i].events = POLLIN | POLLOUT;
            else
                _pollFds[i].events = POLLIN;
        }

        int ret = poll(&_pollFds[0], _pollFds.size(), -1);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                if (g_stopRequested)
                    _running = false;
                continue;
            }
            throw std::runtime_error("poll failed");
        }
        if (_pollFds[0].revents & POLLIN)
            acceptConnection();

        size_t i = 1;
        while (i < _pollFds.size())
        {
            bool removed = handleClientEvent(i);
            if (!removed)
                ++i;
        }
        for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if (now - it->second._lastActivityTime > 120)
            {
                sendToClient(it->second, ":irc.local PING irc.local\r\n");
                it->second._pingWaiting = true;
            } else if (now - it->second._lastActivityTime > 180 && it->second._pingWaiting)
                closeClient(it->first);
        }
    }
}

void Server::acceptConnection()
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    int clientFd = accept(_listenFd, (struct sockaddr *)&addr, &len);
    while (clientFd >= 0)
    {
        len = sizeof(addr);
        if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
        {
            close(clientFd);
            return;
        }

        char host[INET6_ADDRSTRLEN];
        void *src = 0;
        if (addr.ss_family == AF_INET)
            src = &(((struct sockaddr_in *)&addr)->sin_addr);
        else if (addr.ss_family == AF_INET6)
            src = &(((struct sockaddr_in6 *)&addr)->sin6_addr);
        if (src && inet_ntop(addr.ss_family, src, host, sizeof(host)))
        {
            Client client(clientFd);
            client.setHostname(host);
            _clients[clientFd] = client;
            struct pollfd pfd;
            pfd.fd = clientFd;
            pfd.events = POLLIN;
            pfd.revents = 0;
            _pollFds.push_back(pfd);
            Logger::info(std::string("New connection from ") + host + " fd=" + itoa(clientFd));
        }
        else
            close(clientFd);

        len = sizeof(addr);
        clientFd = accept(_listenFd, (struct sockaddr *)&addr, &len);
    }
}

bool Server::handleClientEvent(size_t index)
{
    if (index >= _pollFds.size())
        return false;
    struct pollfd &pfd = _pollFds[index];
    std::map<int, Client>::iterator it = _clients.find(pfd.fd);
    if (it == _clients.end())
    {
        _pollFds.erase(_pollFds.begin() + index);
        return true;
    }
    Client &client = it->second;
    if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL))
    {
        closeClient(client.getFd());
        _pollFds.erase(_pollFds.begin() + index);
        return true;
    }
    if (pfd.revents & POLLIN)
    {
        readFromClient(client);
        if (client.wantsClose())
        {
            closeClient(client.getFd());
            _pollFds.erase(_pollFds.begin() + index);
            return true;
        }
    }
    if (index < _pollFds.size())
    {
        struct pollfd &newPfd = _pollFds[index];
        if (newPfd.revents & POLLOUT)
            flushSendQueue(client, newPfd);
    }
    return false;
}

void Server::closeClient(int fd)
{
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;
    Client &client = it->second;
    removeClientFromChannels(client);
    close(fd);
    Logger::warn("Connection closed fd=" + itoa(fd));
    _clients.erase(it);
}

void Server::readFromClient(Client &client)
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        ssize_t n = recv(client.getFd(), buffer, BUFFER_SIZE, 0);
        if (n > 0)
        {
            client.appendBuffer(std::string(buffer, n));
            std::string line;
            while (client.popLine(line))
            {
                processLine(client, line);
                if (client.wantsClose())
                    return;
            }
        }
        else if (n == 0)
        {
            client.markForClose();
            return;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return;
            client.markForClose();
            return;
        }
    }
}

void Server::flushSendQueue(Client &client, struct pollfd &pfd)
{
    std::string &out = client.pendingSend();
    if (out.empty())
        return;
    ssize_t n = send(client.getFd(), out.c_str(), out.size(), 0);
    if (n > 0)
        out.erase(0, static_cast<std::string::size_type>(n));
    else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        client.markForClose();
    if (out.empty())
        pfd.events = POLLIN;
}

void Server::processLine(Client &client, const std::string &line)
{
    std::string trimmed = trim(line);
    if (trimmed.empty())
        return;
    Logger::debug("fd=" + itoa(client.getFd()) + " raw: " + trimmed);
    std::string command;
    std::vector<std::string> params;

    std::string::size_type pos = 0;
    if (!trimmed.empty() && trimmed[0] == ':')
    {
        std::string::size_type space = trimmed.find(' ');
        if (space != std::string::npos)
            pos = space + 1;
    }
    while (pos < trimmed.size() && trimmed[pos] == ' ')
        ++pos;
    std::string::size_type space = trimmed.find(' ', pos);
    if (space == std::string::npos)
    {
        command = trimmed.substr(pos);
    }
    else
    {
        command = trimmed.substr(pos, space - pos);
        pos = space + 1;
        while (pos < trimmed.size())
        {
            if (trimmed[pos] == ':')
            {
                params.push_back(trimmed.substr(pos + 1));
                break;
            }
            std::string::size_type next = trimmed.find(' ', pos);
            if (next == std::string::npos)
            {
                params.push_back(trimmed.substr(pos));
                break;
            }
            params.push_back(trimmed.substr(pos, next - pos));
            pos = next + 1;
            while (pos < trimmed.size() && trimmed[pos] == ' ')
                ++pos;
        }
    }
    handleCommand(client, toUpper(command), params);
}

void Server::sendNumeric(Client &client, const std::string &code, const std::string &msg)
{
    std::string prefix = ":" + std::string("irc.local");
    std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
    std::string out = prefix + " " + code + " " + nick + " " + msg + "\r\n";
    sendToClient(client, out);
}

void Server::sendFrom(Client &client, const std::string &command, const std::string &params)
{
    std::string prefix = ":" + userPrefix(client);
    std::string out = prefix + " " + command + " " + params + "\r\n";
    sendToClient(client, out);
}

void Server::sendToClient(Client &client, const std::string &msg)
{
    client.queueMessage(msg);
}

void Server::broadcastToChannel(const Channel &channel, const std::string &msg, int skipFd)
{
    const std::set<int> &members = channel.members();
    for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        if (*it == skipFd)
            continue;
        std::map<int, Client>::iterator cit = _clients.find(*it);
        if (cit != _clients.end())
            sendToClient(cit->second, msg);
    }
}

void Server::broadcastUserToChannel(Client &client, Channel &channel, const std::string &command, const std::string &params, int skipFd)
{
    std::string out = ":" + userPrefix(client) + " " + command + " " + params + "\r\n";
    broadcastToChannel(channel, out, skipFd);
}

void Server::ensureChannelHasOperator(Channel &channel)
{
    if (channel.members().empty() || !channel.operators().empty())
        return;
    std::set<int>::const_iterator it = channel.members().begin();
    if (it != channel.members().end())
    {
        int newOpFd = *it;
        channel.grantOperator(newOpFd);
        std::map<int, Client>::iterator cit = _clients.find(newOpFd);
        if (cit != _clients.end())
        {
            const std::string &nick = cit->second.getNickname();
            std::string msg = ":" + std::string("irc.local") + " MODE " + channel.getName() + " +o " + nick + "\r\n";
            broadcastToChannel(channel, msg, -1);
        }
    }
}

std::string Server::userPrefix(const Client &client) const
{
    return client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
}

void Server::authenticateIfReady(Client &client)
{
    if (client.isRegistered())
        return;
    if (!client.hasPassword() || client.getNickname().empty() || client.getUsername().empty())
        return;
    client.setRegistered(true);
    sendNumeric(client, "001", ":Welcome to the IRC server " + userPrefix(client));
    sendNumeric(client, "002", ":Your host is irc.local");
    sendNumeric(client, "003", ":This server was created for ft_irc");
}

bool Server::isNicknameInUse(const std::string &nick, int excludeFd) const
{
    for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (excludeFd != -1 && it->first == excludeFd)
            continue;
        if (toUpper(it->second.getNickname()) == toUpper(nick))
            return true;
    }
    return false;
}

void Server::removeClientFromChannels(Client &client)
{
    const std::set<std::string> &joined = client.getChannels();
    for (std::set<std::string>::const_iterator it = joined.begin(); it != joined.end(); ++it)
    {
        std::map<std::string, Channel>::iterator chIt = _channels.find(*it);
        if (chIt != _channels.end())
        {
            Channel &channel = chIt->second;
            broadcastUserToChannel(client, channel, "PART", channel.getName(), client.getFd());
            channel.removeMember(client.getFd());
            ensureChannelHasOperator(channel);
            if (channel.members().empty())
                _channels.erase(chIt);
        }
    }
}

Channel *Server::findChannel(const std::string &name)
{
    std::map<std::string, Channel>::iterator it = _channels.find(name);
    if (it == _channels.end())
        return 0;
    return &it->second;
}

Client *Server::findClientByNick(const std::string &nick)
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (toUpper(it->second.getNickname()) == toUpper(nick))
            return &it->second;
    }
    return 0;
}

std::string Server::channelModeString(const Channel &channel) const
{
    std::string modes = "+";
    std::string args;
    if (channel.isInviteOnly())
        modes += "i";
    if (channel.isTopicRestricted())
        modes += "t";
    if (channel.hasKey())
    {
        modes += "k";
        args += " " + channel.getKey();
    }
    if (channel.hasLimit())
    {
        modes += "l";
        args += " " + itoa(static_cast<int>(channel.getLimit()));
    }
    return modes + args;
}

bool Server::checkRegistrationOrder(Client &client, const std::string &cmd)
{
    if (cmd == "CAP")
        return false;
    if (!client.isRegistered())
    {
        if (cmd != "PASS" && cmd != "NICK" && cmd != "USER" && cmd != "QUIT" && cmd != "PING")
        {
            sendNumeric(client, "451", ":You are not registered");
            return true;
        }
    }
    return false;
}


//signal ctrl z
