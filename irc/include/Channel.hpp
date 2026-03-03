#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <string>
#include <vector>

class Channel
{
public:
    Channel();
    explicit Channel(const std::string &name);

    const std::string &getName() const;
    const std::string &getTopic() const;

    bool isInviteOnly() const;
    bool isTopicRestricted() const;
    bool hasKey() const;
    bool hasLimit() const;
    const std::string &getKey() const;
    size_t getLimit() const;

    void setTopic(const std::string &topic);
    void clearTopic();

    void setInviteOnly(bool value);
    void setTopicRestricted(bool value);
    void setKey(const std::string &key);
    void removeKey();
    void setLimit(size_t limit);
    void removeLimit();

    void addMember(int fd, bool op);
    void removeMember(int fd);
    bool isMember(int fd) const;
    bool isOperator(int fd) const;
    void grantOperator(int fd);
    void revokeOperator(int fd);

    const std::set<int> &members() const;
    const std::set<int> &operators() const;

    void invite(int fd);
    bool isInvited(int fd) const;
    void removeInvite(int fd);

private:
    std::string _name;
    std::string _topic;
    bool _inviteOnly;
    bool _topicRestricted;
    bool _hasKey;
    std::string _key;
    bool _hasLimit;
    size_t _userLimit;
    std::set<int> _members;
    std::set<int> _operators;
    std::set<int> _invited;
};

#endif
