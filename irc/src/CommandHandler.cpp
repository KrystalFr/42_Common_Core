#include "Server.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

#include <cstdlib>
#include <iostream>
#include <ctime>

void Server::handleCommand(Client &client, const std::string &cmd, const std::vector<std::string> &params)
{
    if (checkRegistrationOrder(client, cmd))
        return;
    Logger::debug("fd=" + itoa(client.getFd()) + " cmd=" + cmd);

    if (cmd == "PASS")
        handlePass(client, params);
    else if (cmd == "NICK")
        handleNick(client, params);
    else if (cmd == "USER")
        handleUser(client, params);
    else if (cmd == "PING")
        handlePing(client, params);
    else if (cmd == "PONG")
        handlePong(client, params);
    else if (cmd == "JOIN")
        handleJoin(client, params);
    else if (cmd == "PRIVMSG")
        handlePrivmsg(client, params);
    else if (cmd == "PART")
        handlePart(client, params);
    else if (cmd == "QUIT")
        handleQuit(client, params);
    else if (cmd == "KICK")
        handleKick(client, params);
    else if (cmd == "INVITE")
        handleInvite(client, params);
    else if (cmd == "TOPIC")
        handleTopic(client, params);
    else if (cmd == "MODE")
        handleMode(client, params);
    else if (cmd == "WHOIS" || cmd == "WHO")
        handleWhois(client, params);
    else if (cmd == "CAP")
        handleCap(client, params);
    else
        sendNumeric(client, "421", cmd + " :Unknown command");
}

void Server::handlePass(Client &client, const std::vector<std::string> &params)
{
    if (client.isRegistered())
    {
        sendNumeric(client, "462", ":You may not reregister");
        return;
    }
    if (params.empty())
    {
        sendNumeric(client, "461", "PASS :Not enough parameters");
        return;
    }
    if (params[0] != _password)
    {
        sendNumeric(client, "464", ":Password incorrect");
        client.markForClose();
        return;
    }
    client.setPasswordAccepted(true);
    authenticateIfReady(client);
}

void Server::handleNick(Client &client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        sendNumeric(client, "431", ":No nickname given");
        return;
    }
    const std::string &nick = params[0];
    if (!isValidNickname(nick))
    {
        sendNumeric(client, "432", nick + " :Erroneous nickname");
        return;
    }
    if (isNicknameInUse(nick, client.getFd()))
    {
        sendNumeric(client, "433", nick + " :Nickname is already in use");
        return;
    }
    std::string oldNick = client.getNickname();
    client.setNickname(nick);
    if (!oldNick.empty() && oldNick != nick)
    {
        const std::set<std::string> &channels = client.getChannels();
        for (std::set<std::string>::const_iterator it = channels.begin(); it != channels.end(); ++it)
        {
            Channel *channel = findChannel(*it);
            if (channel)
                broadcastUserToChannel(client, *channel, "NICK", ":" + nick, client.getFd());
        }
        sendFrom(client, "NICK", ":" + nick);
    }
    authenticateIfReady(client);
}

void Server::handleUser(Client &client, const std::vector<std::string> &params)
{
    if (client.isRegistered())
    {
        sendNumeric(client, "462", ":You may not reregister");
        return;
    }
    if (params.size() < 4)
    {
        sendNumeric(client, "461", "USER :Not enough parameters");
        return;
    }
    client.setUsername(params[0]);
    client.setRealname(params[3]);
    authenticateIfReady(client);
}

void Server::handlePing(Client &client, const std::vector<std::string> &params)
{
    if (params.empty())
        sendNumeric(client, "409", ":No origin specified");
    else
        sendToClient(client, ":" + std::string("irc.local") + " PONG irc.local :" + params[0] + "\r\n");
}

void Server::handlePong(Client &client, const std::vector<std::string> &params)
{
    (void)params;
    client._pingWaiting = false;
    client._lastActivityTime = std::time(0);
}

void Server::handleJoin(Client &client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        sendNumeric(client, "461", "JOIN :Not enough parameters");
        return;
    }
    std::vector<std::string> chans = split(params[0], ',');
    std::vector<std::string> keys;
    if (params.size() > 1)
        keys = split(params[1], ',');

    for (size_t i = 0; i < chans.size(); ++i)
    {
        std::string name = chans[i];
        std::string key = (i < keys.size()) ? keys[i] : "";
        if (!isChannelName(name))
        {
            sendNumeric(client, "403", name + " :No such channel");
            continue;
        }
        Channel *channel = findChannel(name);
        if (!channel)
        {
            Channel created(name);
            _channels[name] = created;
            channel = &_channels[name];
        }
        if (channel->isMember(client.getFd()))
            continue;
        if (channel->isInviteOnly() && !channel->isInvited(client.getFd()))
        {
            sendNumeric(client, "473", name + " :Cannot join channel (+i)");
            continue;
        }
        if (channel->hasKey() && channel->getKey() != key)
        {
            sendNumeric(client, "475", name + " :Cannot join channel (+k)");
            continue;
        }
        if (channel->hasLimit() && channel->members().size() >= channel->getLimit())
        {
            sendNumeric(client, "471", name + " :Channel is full");
            continue;
        }
        bool op = channel->members().empty();
        channel->addMember(client.getFd(), op);
        channel->removeInvite(client.getFd());
        client.addChannel(name);
        std::string joinMsg = ":" + userPrefix(client) + " JOIN " + name + "\r\n";
        broadcastToChannel(*channel, joinMsg, -1);
        if (channel->getTopic().empty())
            sendNumeric(client, "331", name + " :No topic is set");
        else
            sendNumeric(client, "332", name + " :" + channel->getTopic());

        // Names reply
        std::string names;
        const std::set<int> &members = channel->members();
        for (std::set<int>::const_iterator m = members.begin(); m != members.end(); ++m)
        {
            std::map<int, Client>::iterator cit = _clients.find(*m);
            if (cit == _clients.end())
                continue;
            if (!names.empty())
                names += " ";
            if (channel->isOperator(*m))
                names += "@";
            names += cit->second.getNickname();
        }
        sendNumeric(client, "353", "= " + name + " :" + names);
        sendNumeric(client, "366", name + " :End of NAMES list");
    }
}

void Server::handlePrivmsg(Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        sendNumeric(client, "461", "PRIVMSG :Not enough parameters");
        return;
    }
    std::string target = params[0];
    std::string message = params[1];
    if (message.empty())
    {
        sendNumeric(client, "412", ":No text to send");
        return;
    }
    if (isChannelName(target))
    {
        Channel *channel = findChannel(target);
        if (!channel)
        {
            sendNumeric(client, "403", target + " :No such channel");
            return;
        }
        if (!channel->isMember(client.getFd()))
        {
            sendNumeric(client, "404", target + " :Cannot send to channel");
            return;
        }
        std::string out = ":" + userPrefix(client) + " PRIVMSG " + target + " :" + message + "\r\n";
        broadcastToChannel(*channel, out, client.getFd());
    }
    else
    {
        Client *targetClient = findClientByNick(target);
        if (!targetClient)
        {
            sendNumeric(client, "401", target + " :No such nick");
            return;
        }
        std::string out = ":" + userPrefix(client) + " PRIVMSG " + target + " :" + message + "\r\n";
        sendToClient(*targetClient, out);
    }
}

void Server::handlePart(Client &client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        sendNumeric(client, "461", "PART :Not enough parameters");
        return;
    }
    std::vector<std::string> chans = split(params[0], ',');
    for (size_t i = 0; i < chans.size(); ++i)
    {
        Channel *channel = findChannel(chans[i]);
        if (!channel)
        {
            sendNumeric(client, "403", chans[i] + " :No such channel");
            continue;
        }
        if (!channel->isMember(client.getFd()))
        {
            sendNumeric(client, "442", chans[i] + " :You're not on that channel");
            continue;
        }
        std::string reason = (params.size() > 1) ? params[1] : "Leaving";
        std::string msg = ":" + userPrefix(client) + " PART " + chans[i] + " :" + reason + "\r\n";
        broadcastToChannel(*channel, msg, -1);
        channel->removeMember(client.getFd());
        client.removeChannel(chans[i]);
        if (channel->members().empty())
            _channels.erase(chans[i]);
        else
            ensureChannelHasOperator(*channel);
    }
}

void Server::handleQuit(Client &client, const std::vector<std::string> &params)
{
    std::string reason = params.empty() ? "Quit" : params[0];
    std::string msg = ":" + userPrefix(client) + " QUIT :" + reason + "\r\n";
    const std::set<std::string> &joined = client.getChannels();
    for (std::set<std::string>::const_iterator it = joined.begin(); it != joined.end(); ++it)
    {
        Channel *channel = findChannel(*it);
        if (channel)
            broadcastToChannel(*channel, msg, client.getFd());
    }
    Logger::info("Client quitting fd=" + itoa(client.getFd()) + " reason=" + reason);
    client.markForClose();
}

void Server::handleKick(Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        sendNumeric(client, "461", "KICK :Not enough parameters");
        return;
    }
    std::vector<std::string> chans = split(params[0], ',');
    std::vector<std::string> users = split(params[1], ',');
    for (size_t i = 0; i < chans.size(); ++i)
    {
        std::string chanName = chans[i];
        Channel *channel = findChannel(chanName);
        if (!channel)
        {
            sendNumeric(client, "403", chanName + " :No such channel");
            continue;
        }
        if (!channel->isMember(client.getFd()))
        {
            sendNumeric(client, "442", chanName + " :You're not on that channel");
            continue;
        }
        if (!channel->isOperator(client.getFd()))
        {
            sendNumeric(client, "482", chanName + " :You're not channel operator");
            continue;
        }
        std::string target = (i < users.size()) ? users[i] : users[0];
        Client *targetClient = findClientByNick(target);
        if (!targetClient || !channel->isMember(targetClient->getFd()))
        {
            sendNumeric(client, "441", target + " " + chanName + " :They aren't on that channel");
            continue;
        }
        std::string reason = params.size() > 2 ? params[2] : client.getNickname();
        std::string msg = ":" + userPrefix(client) + " KICK " + chanName + " " + target + " :" + reason + "\r\n";
        broadcastToChannel(*channel, msg, -1);
        channel->removeMember(targetClient->getFd());
        targetClient->removeChannel(chanName);
        if (channel->members().empty())
            _channels.erase(chanName);
        else
            ensureChannelHasOperator(*channel);
    }
}

void Server::handleInvite(Client &client, const std::vector<std::string> &params)
{
    if (params.size() < 2)
    {
        sendNumeric(client, "461", "INVITE :Not enough parameters");
        return;
    }
    std::string nick = params[0];
    std::string chanName = params[1];
    Client *target = findClientByNick(nick);
    if (!target)
    {
        sendNumeric(client, "401", nick + " :No such nick");
        return;
    }
    Channel *channel = findChannel(chanName);
    if (!channel)
    {
        sendNumeric(client, "403", chanName + " :No such channel");
        return;
    }
    if (!channel->isMember(client.getFd()))
    {
        sendNumeric(client, "442", chanName + " :You're not on that channel");
        return;
    }
    if (!channel->isOperator(client.getFd()))
    {
        sendNumeric(client, "482", chanName + " :You're not channel operator");
        return;
    }
    if (channel->isMember(target->getFd()))
    {
        sendNumeric(client, "443", nick + " " + chanName + " :is already on channel");
        return;
    }
    channel->invite(target->getFd());
    sendNumeric(client, "341", nick + " " + chanName);
    std::string msg = ":" + userPrefix(client) + " INVITE " + nick + " :" + chanName + "\r\n";
    sendToClient(*target, msg);
}

void Server::handleTopic(Client &client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        sendNumeric(client, "461", "TOPIC :Not enough parameters");
        return;
    }
    std::string chanName = params[0];
    Channel *channel = findChannel(chanName);
    if (!channel)
    {
        sendNumeric(client, "403", chanName + " :No such channel");
        return;
    }
    if (!channel->isMember(client.getFd()))
    {
        sendNumeric(client, "442", chanName + " :You're not on that channel");
        return;
    }
    if (params.size() == 1)
    {
        if (channel->getTopic().empty())
            sendNumeric(client, "331", chanName + " :No topic is set");
        else
            sendNumeric(client, "332", chanName + " :" + channel->getTopic());
        return;
    }
    if (channel->isTopicRestricted() && !channel->isOperator(client.getFd()))
    {
        sendNumeric(client, "482", chanName + " :You're not channel operator");
        return;
    }
    channel->setTopic(params[1]);
    std::string msg = ":" + userPrefix(client) + " TOPIC " + chanName + " :" + params[1] + "\r\n";
    broadcastToChannel(*channel, msg, -1);
}

void Server::handleMode(Client &client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        sendNumeric(client, "461", "MODE :Not enough parameters");
        return;
    }
    std::string target = params[0];
    if (target == client.getNickname())
        return;

    if (!isChannelName(target))
    {
        if (params.size() == 1)
        {
            if (toUpper(target) == toUpper(client.getNickname()))
                sendNumeric(client, "221", "+");
            else
                sendNumeric(client, "502", target + " :Cannot view or change modes for other users");
        }
        else
            sendNumeric(client, "502", target + " :Cannot change user modes");
        return;
    }
    Channel *channel = findChannel(target);
    if (!channel)
    {
        sendNumeric(client, "403", target + " :No such channel");
        return;
    }
    if (params.size() == 1)
    {
        sendNumeric(client, "324", target + " " + channelModeString(*channel));
        return;
    }
    if (!channel->isMember(client.getFd()))
    {
        sendNumeric(client, "442", target + " :You're not on that channel");
        return;
    }
    if (!channel->isOperator(client.getFd()))
    {
        sendNumeric(client, "482", target + " :You're not channel operator");
        return;
    }

    std::string modeStr = params[1];
    bool add = true;
    char currentSign = '\0';
    bool signApplied = false;
    bool signSeen = false;
    bool invalidMode = false;
    size_t argIndex = 2;
    std::string appliedModes;
    std::string appliedArgs;

    for (size_t i = 0; i < modeStr.size(); ++i)
    {
        char m = modeStr[i];
        if (m == '+')
        {
            add = true;
            currentSign = '+';
            signSeen = true;
            signApplied = false;
            continue;
        }
        if (m == '-')
        {
            add = false;
            currentSign = '-';
            signSeen = true;
            signApplied = false;
            continue;
        }
        if (!signSeen)
        {
            sendNumeric(client, "472", std::string(1, m) + " :is unknown mode char to me for " + target);
            invalidMode = true;
            break;
        }

        if (m == 'i')
        {
            if (!signApplied)
            {
                appliedModes += currentSign;
                signApplied = true;
            }
            channel->setInviteOnly(add);
            appliedModes += "i";
        }
        else if (m == 't')
        {
            if (!signApplied)
            {
                appliedModes += currentSign;
                signApplied = true;
            }
            channel->setTopicRestricted(add);
            appliedModes += "t";
        }
        else if (m == 'k')
        {
            if (add)
            {
                if (argIndex >= params.size())
                {
                    sendNumeric(client, "461", "MODE :Not enough parameters");
                    break;
                }
                if (!signApplied)
                {
                    appliedModes += currentSign;
                    signApplied = true;
                }
                channel->setKey(params[argIndex]);
                appliedModes += "k";
                appliedArgs += " " + params[argIndex];
                ++argIndex;
            }
            else
            {
                if (!signApplied)
                {
                    appliedModes += currentSign;
                    signApplied = true;
                }
                channel->removeKey();
                appliedModes += "k";
            }
        }
        else if (m == 'o')
        {
            if (argIndex >= params.size())
            {
                sendNumeric(client, "461", "MODE :Not enough parameters");
                break;
            }
            Client *targetClient = findClientByNick(params[argIndex]);
            if (!targetClient || !channel->isMember(targetClient->getFd()))
            {
                sendNumeric(client, "441", params[argIndex] + " " + target + " :They aren't on that channel");
                ++argIndex;
                continue;
            }
            if (add)
                channel->grantOperator(targetClient->getFd());
            else
                channel->revokeOperator(targetClient->getFd());
            if (!signApplied)
            {
                appliedModes += currentSign;
                signApplied = true;
            }
            appliedModes += "o";
            appliedArgs += " " + params[argIndex];
            ++argIndex;
        }
        else if (m == 'l')
        {
            if (add)
            {
                if (argIndex >= params.size())
                {
                    sendNumeric(client, "461", "MODE :Not enough parameters");
                    break;
                }
                int limit = std::atoi(params[argIndex].c_str());
                if (limit > 0)
                {
                    if (!signApplied)
                    {
                        appliedModes += currentSign;
                        signApplied = true;
                    }
                    channel->setLimit(static_cast<size_t>(limit));
                    appliedModes += "l";
                    appliedArgs += " " + params[argIndex];
                }
                ++argIndex;
            }
            else
            {
                if (!signApplied)
                {
                    appliedModes += currentSign;
                    signApplied = true;
                }
                channel->removeLimit();
                appliedModes += "l";
            }
        }
        else
        {
            sendNumeric(client, "472", std::string(1, m) + " :is unknown mode char to me for " + target);
            invalidMode = true;
            break;
        }
    }
    if (!invalidMode && !appliedModes.empty())
    {
        std::string msg = ":" + userPrefix(client) + " MODE " + target + " " + appliedModes + appliedArgs + "\r\n";
        broadcastToChannel(*channel, msg, -1);
    }
}

void Server::handleCap(Client &client, const std::vector<std::string> &params)
{
    (void)params;
    sendToClient(client, "CAP * LS :\r\n");
}

void Server::handleWhois(Client &client, const std::vector<std::string> &params)
{
    if (params.empty())
    {
        sendNumeric(client, "431", ":No nickname given");
        return;
    }
    std::string target = params[0];
    Client *found = findClientByNick(target);
    if (!found)
    {
        sendNumeric(client, "401", target + " :No such nick");
        sendNumeric(client, "318", target + " :End of /WHOIS list");
        return;
    }
    sendNumeric(client, "311", found->getNickname() + " " + found->getUsername() + " " + found->getHostname() + " * :" + found->getRealname());
    sendNumeric(client, "318", found->getNickname() + " :End of /WHOIS list");
}
