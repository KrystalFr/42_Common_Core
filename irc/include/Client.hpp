#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <set>
#include <string>
#include <ctime>

class Client
{
public:
    Client();
    explicit Client(int fd);

    int getFd() const;
    const std::string &getNickname() const;
    const std::string &getUsername() const;
    const std::string &getRealname() const;
    const std::string &getHostname() const;
    bool isRegistered() const;
    bool hasPassword() const;
    bool wantsClose() const;

    void setHostname(const std::string &host);
    void setPasswordAccepted(bool ok);
    void setNickname(const std::string &nick);
    void setUsername(const std::string &user);
    void setRealname(const std::string &real);
    void setRegistered(bool value);
    void markForClose();

    void appendBuffer(const std::string &data);
    bool popLine(std::string &line);

    void queueMessage(const std::string &msg);
    bool hasPendingSend() const;
    std::string &pendingSend();

    void addChannel(const std::string &name);
    void removeChannel(const std::string &name);
    const std::set<std::string> &getChannels() const;

    time_t _lastActivityTime;
    bool _pingWaiting;

private:
    int _fd;
    std::string _hostname;
    std::string _buffer;
    std::string _out;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    bool _passwordOk;
    bool _registered;
    bool _close;
    std::set<std::string> _channels;
};

#endif
