#include "Channel.hpp"

Channel::Channel() : _inviteOnly(false), _topicRestricted(false), _hasKey(false), _hasLimit(false), _userLimit(0)
{
}

Channel::Channel(const std::string &name) : _name(name), _inviteOnly(false), _topicRestricted(false), _hasKey(false), _hasLimit(false), _userLimit(0)
{
}

const std::string &Channel::getName() const { return _name; }
const std::string &Channel::getTopic() const { return _topic; }
bool Channel::isInviteOnly() const { return _inviteOnly; }
bool Channel::isTopicRestricted() const { return _topicRestricted; }
bool Channel::hasKey() const { return _hasKey; }
bool Channel::hasLimit() const { return _hasLimit; }
const std::string &Channel::getKey() const { return _key; }
size_t Channel::getLimit() const { return _userLimit; }

void Channel::setTopic(const std::string &topic) { _topic = topic; }
void Channel::clearTopic() { _topic.clear(); }
void Channel::setInviteOnly(bool value) { _inviteOnly = value; }
void Channel::setTopicRestricted(bool value) { _topicRestricted = value; }
void Channel::setKey(const std::string &key)
{
    _hasKey = true;
    _key = key;
}
void Channel::removeKey()
{
    _hasKey = false;
    _key.clear();
}
void Channel::setLimit(size_t limit)
{
    _hasLimit = true;
    _userLimit = limit;
}
void Channel::removeLimit()
{
    _hasLimit = false;
    _userLimit = 0;
}

void Channel::addMember(int fd, bool op)
{
    _members.insert(fd);
    if (op)
        _operators.insert(fd);
}

void Channel::removeMember(int fd)
{
    _members.erase(fd);
    _operators.erase(fd);
    _invited.erase(fd);
}

bool Channel::isMember(int fd) const
{
    return _members.find(fd) != _members.end();
}

bool Channel::isOperator(int fd) const
{
    return _operators.find(fd) != _operators.end();
}

void Channel::grantOperator(int fd)
{
    if (isMember(fd))
        _operators.insert(fd);
}

void Channel::revokeOperator(int fd)
{
    _operators.erase(fd);
}

const std::set<int> &Channel::members() const { return _members; }
const std::set<int> &Channel::operators() const { return _operators; }

void Channel::invite(int fd)
{
    _invited.insert(fd);
}

bool Channel::isInvited(int fd) const
{
    return _invited.find(fd) != _invited.end();
}

void Channel::removeInvite(int fd)
{
    _invited.erase(fd);
}
