#include "../include/Channel.hpp"

Channel::Channel(const std::string& name)
    : _name(name), _topic(""), _key(""), _inviteOnly(false), _topicOpOnly(false), _limit(0)
{
}

Channel::Channel(const Channel& other)
    : _name(other._name), _topic(other._topic), _key(other._key), _inviteOnly(other._inviteOnly), _topicOpOnly(other._topicOpOnly), _limit(other._limit)
{
}

Channel& Channel::operator=(const Channel& other)
{
    if (this != &other)
    {
        _name = other._name;
        _topic = other._topic;
        _key = other._key;
        _inviteOnly = other._inviteOnly;
        _topicOpOnly = other._topicOpOnly;
        _limit = other._limit;
    }
    return *this;
}

Channel::~Channel()
{
}

const std::string&	Channel::getName() const
{
    return _name;
}

const std::string&	Channel::getTopic() const
{
    return _topic;
}

const std::string&	Channel::getKey() const
{
    return _key;
}

bool				Channel::isInviteOnly() const
{
    return _inviteOnly;
}

bool				Channel::isTopicOpOnly() const
{
    return _topicOpOnly;
}
bool				Channel::hasKey() const
{
    return !_key.empty();
}
bool				Channel::hasLimit() const
{
    return _limit > 0;
}
size_t				Channel::getLimit() const
{
    return _limit;
}

size_t				Channel::memberCount() const
{
    return _members.size();
}

bool				Channel::isMember(const Client& client) const
{
    return _members.find(client.getClientFd()) != _members.end();
}

bool				Channel::isOperator(const Client& client) const
{
    return _operators.find(client.getClientFd()) != _operators.end();
}

bool				Channel::isInvited(const Client& client) const
{
    return _invited.find(client.getClientFd()) != _invited.end();
}

void	Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}

void	Channel::setKey(const std::string& key)
{
    _key = key;
}

void	Channel::removeKey()
{
    _key = "";
}

void	Channel::setInviteOnly(bool value)
{
    _inviteOnly = value;
}

void	Channel::setTopicOpOnly(bool value)
{
    _topicOpOnly = value;
}

void	Channel::setLimit(size_t limit)
{
    _limit = limit;
}
void	Channel::removeLimit()
{
    _limit = 0;
}

bool	Channel::addMember(Client& client)
{
    if (isMember(client))
        return false;
    _members[client.getClientFd()] = &client;
    return true;
}
bool	Channel::removeMember(Client& client)
{
    std::map<int, Client*>::iterator it = _members.find(client.getClientFd());
    if (it == _members.end())
        return false;
    _members.erase(it);
    removeOperator(client);
    return true;
}

bool	Channel::addOperator(Client& client)
{
    if (isOperator(client))
        return false;
    _operators[client.getClientFd()] = &client;
    return true;
}
bool	Channel::removeOperator(Client& client)
{
    std::map<int, Client*>::iterator it = _operators.find(client.getClientFd());
    if (it == _operators.end())
        return false;
    _operators.erase(it);
    return true;
}

bool	Channel::addInvitation(Client& client)
{
    if (isInvited(client))
        return false;
    _invited[client.getClientFd()] = &client;
    return true;
}
bool	Channel::removeInvitation(Client& client)
{
    std::map<int, Client*>::iterator it = _invited.find(client.getClientFd());
    if (it == _invited.end())
        return false;
    _invited.erase(it);
    return true;
}

bool	Channel::isFull() const
{
    return memberCount() >= getLimit();
}
bool	Channel::isKeyValid(const std::string& key) const
{
    return hasKey() && _key == key;
}

void	Channel::broadcastToChannel(Server& server, const std::string& message, const Client& sender, int flagEx) const
{
    if(flagEx == 1)
    {
        for (std::map<int, Client*>::const_iterator it = this->_members.begin();
                it != this->_members.end(); ++it) 
        {
            if (it->second->getClientFd() != sender.getClientFd())
            {
                server.sendToClient(*(it->second), message);
            }
        }
    }
    else
    {
        for (std::map<int, Client*>::const_iterator it = this->_members.begin();
                it != this->_members.end(); ++it) 
        {
            server.sendToClient(*(it->second), message);
        }
    }
}

void	Channel::sendNamesList(Server& server, Client& client) const
{
    std::string namesList = "";
    for (std::map<int, Client*>::const_iterator it = _members.begin();
                it != _members.end(); ++it) 
    {
        if (isOperator(*(it->second)))
            namesList += "@" + it->second->getNickName() + " ";
        else
            namesList += it->second->getNickName() + " ";
    }
    sendReply(client, server, RPL_NAMREPLY, "= " + _name + " :" + namesList);
    sendReply(client, server, RPL_ENDOFNAMES, _name);
}