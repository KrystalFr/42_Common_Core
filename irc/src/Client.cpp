#include "Client.hpp"

#include <algorithm>
#include <ctime>

Client::Client() : _fd(-1), _passwordOk(false), _registered(false), _close(false)
{
    _lastActivityTime = std::time(0);
    _pingWaiting = false;
}

Client::Client(int fd) : _fd(fd), _passwordOk(false), _registered(false), _close(false)
{
    _lastActivityTime = std::time(0);
    _pingWaiting = false;
}

int Client::getFd() const { return _fd; }
const std::string &Client::getNickname() const { return _nickname; }
const std::string &Client::getUsername() const { return _username; }
const std::string &Client::getRealname() const { return _realname; }
const std::string &Client::getHostname() const { return _hostname; }
bool Client::isRegistered() const { return _registered; }
bool Client::hasPassword() const { return _passwordOk; }
bool Client::wantsClose() const { return _close; }

void Client::setHostname(const std::string &host) { _hostname = host; }
void Client::setPasswordAccepted(bool ok) { _passwordOk = ok; }
void Client::setNickname(const std::string &nick) { _nickname = nick; }
void Client::setUsername(const std::string &user) { _username = user; }
void Client::setRealname(const std::string &real) { _realname = real; }
void Client::setRegistered(bool value) { _registered = value; }
void Client::markForClose() { _close = true; }

void Client::appendBuffer(const std::string &data)
{
    _buffer += data;
    _lastActivityTime = std::time(0);
    _pingWaiting = false;
}

bool Client::popLine(std::string &line)
{
    std::string::size_type pos = _buffer.find('\n');
    if (pos == std::string::npos)
        return false;
    line = _buffer.substr(0, pos);
    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    _buffer.erase(0, pos + 1);
    return true;
}

void Client::queueMessage(const std::string &msg)
{
    _out += msg;
}

bool Client::hasPendingSend() const
{
    return !_out.empty();
}

std::string &Client::pendingSend()
{
    return _out;
}

void Client::addChannel(const std::string &name)
{
    _channels.insert(name);
}

void Client::removeChannel(const std::string &name)
{
    _channels.erase(name);
}

const std::set<std::string> &Client::getChannels() const
{
    return _channels;
}
