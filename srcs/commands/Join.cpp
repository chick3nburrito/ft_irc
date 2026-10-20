

#include "../../include/CommandDispatcher.hpp"

std::string getChannelName(const std::string &channel)
{
    if(channel.empty())
        return ("");
    if(channel[0] != '#' && channel[0] != '&')
        return ("");
    return (channel);
}

bool checkJoinCondition(Client &client, Server &server, Channel *channel, const std::string &key)
{
    if(channel->isInviteOnly())
    {
        if (!channel->isInvited(client))
            return (sendReply(client, server, ERR_INVITEONLYCHAN, channel->getName() + " :Cannot join channel (+i)"), false);
    }
    if(channel->hasKey())
    {
        if(!key.empty())
        {
            if(!channel->isKeyValid(key))
                return (sendReply(client, server, ERR_BADCHANNELKEY, channel->getName() + " :Cannot join channel (+k)"), false);
        }
        else
            return (sendReply(client, server, ERR_BADCHANNELKEY, channel->getName() + " :Cannot join channel (+k)"), false);
    }
    if(channel->hasLimit() && channel->isFull())
        return (sendReply(client, server, ERR_CHANNELISFULL, channel->getName() + " :Cannot join channel (+l)"), false);
    return (true);
}

void CommandDispatcher::executeJOIN(Client &client, Server &server, const IrcMessage &message)
{
    if(!client.getRegistrationStatus())
        return (sendReply(client, server, ERR_NOTREGISTERED, ":You are not registered"));
    if(message.params.empty())
    {
        return (sendReply(client, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters"));
    }
  	std::vector<std::string> Channels = splitDelimiter(message.params[0], ',');
    std::vector<std::string> keys;
    if(message.params.size() > 1)
    {
        keys = splitDelimiter(message.params[1], ',');
    }
    std::string channelName;
    Channel *channel;
    std::string key;
    for (size_t i = 0; i < Channels.size(); i++)
    {
        channelName = getChannelName(Channels[i]);
        if(channelName.empty())
        {
            sendReply(client, server, ERR_NOSUCHCHANNEL, Channels[i] + " :No such channel");
            continue;
        }
        else if(!server.getChannel(channelName))
        {
            server.addChannel(channelName);
            channel = server.getChannel(channelName);
            channel->addMember(client);
            channel->addOperator(client);
            channel->broadcastToChannel(server, UserPrefix(client) + message.command + " " + channelName, client, 0);
            if(!channel->getTopic().empty())
                sendReply(client, server, RPL_TOPIC, channelName + " :" + channel->getTopic());
            channel->sendNamesList(server, client);
        }
        else if(server.getChannel(channelName))
        {
            channel = server.getChannel(channelName);
            key = (i < keys.size()) ? keys[i] : "";
            if(!checkJoinCondition(client, server, channel, key))
                continue;
            if(channel->isMember(client))
                continue;
            channel->addMember(client);
            channel->removeInvitation(client);
            channel->broadcastToChannel(server, UserPrefix(client) + message.command + " " + channelName, client, 0);
            if(!channel->getTopic().empty())
                sendReply(client, server, RPL_TOPIC, channelName + " :" + channel->getTopic());
            channel->sendNamesList(server, client);
        }
    }
}
