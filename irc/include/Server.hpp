#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include "Client.hpp"

#include <map>
#include <poll.h>
#include <string>
#include <vector>

class Server
{
public:
    Server(int port, const std::string &password);
    ~Server();

    void run();

private:
    Server(const Server &);
    Server &operator=(const Server &);

    int _port;
    std::string _password;
    int _listenFd;
    bool _running;
    std::vector<struct pollfd> _pollFds;
    std::map<int, Client> _clients;
    std::map<std::string, Channel> _channels;

    int setupListener();
    void setupSignalHandlers();
    void stop();
    void mainLoop();
    void acceptConnection();
    bool handleClientEvent(size_t index);
    void closeClient(int fd);

    void readFromClient(Client &client);
    void flushSendQueue(Client &client, struct pollfd &pfd);
    void processLine(Client &client, const std::string &line);
    void authenticateIfReady(Client &client);
    void sendNumeric(Client &client, const std::string &code, const std::string &msg);
    void sendFrom(Client &client, const std::string &command, const std::string &params);
    void sendToClient(Client &client, const std::string &msg);
    void broadcastToChannel(const Channel &channel, const std::string &msg, int skipFd);
    void broadcastUserToChannel(Client &client, Channel &channel, const std::string &command, const std::string &params, int skipFd);
    void ensureChannelHasOperator(Channel &channel);

    void handleCommand(Client &client, const std::string &cmd, const std::vector<std::string> &params);
    void handlePass(Client &client, const std::vector<std::string> &params);
    void handleNick(Client &client, const std::vector<std::string> &params);
    void handleUser(Client &client, const std::vector<std::string> &params);
    void handlePing(Client &client, const std::vector<std::string> &params);
    void handlePong(Client &client, const std::vector<std::string> &params);
    void handleJoin(Client &client, const std::vector<std::string> &params);
    void handlePrivmsg(Client &client, const std::vector<std::string> &params);
    void handlePart(Client &client, const std::vector<std::string> &params);
    void handleQuit(Client &client, const std::vector<std::string> &params);
    void handleKick(Client &client, const std::vector<std::string> &params);
    void handleInvite(Client &client, const std::vector<std::string> &params);
    void handleTopic(Client &client, const std::vector<std::string> &params);
    void handleMode(Client &client, const std::vector<std::string> &params);
    void handleCap(Client &client, const std::vector<std::string> &params);
    void handleWhois(Client &client, const std::vector<std::string> &params);

    bool isNicknameInUse(const std::string &nick, int excludeFd = -1) const;
    void removeClientFromChannels(Client &client);
    Channel *findChannel(const std::string &name);
    Client *findClientByNick(const std::string &nick);
    std::string userPrefix(const Client &client) const;
    std::string channelModeString(const Channel &channel) const;
    bool checkRegistrationOrder(Client &client, const std::string &cmd);
};

#endif
