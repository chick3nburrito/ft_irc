
#include "../../include/CommandDispatcher.hpp"

void CommandDispatcher::executeTOPIC(Client &client, Server &server, const IrcMessage &message)
{
    if (!client.getRegistrationStatus())
        return (sendReply(client, server, ERR_NOTREGISTERED, ":You are not registered"));

    if (message.params.size() < 1)
        return (sendReply(client, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters"));

    std::string channelName = message.params[0];
    Channel *channel = server.getChannel(channelName);
    if (!channel)
        return (sendReply(client, server, ERR_NOSUCHCHANNEL, channelName + " :No such channel"));
    if (message.params.size() == 1)
    {
        std::string topic = channel->getTopic();
        if (topic.empty())
            return (sendReply(client, server, RPL_NOTOPIC, channelName + " :No topic is set"));
        else
            return (sendReply(client, server, RPL_TOPIC, channelName + " :" + topic));
    }

    if (!channel->isMember(client))
        return (sendReply(client, server, ERR_NOTONCHANNEL, channelName + " :You're not on that channel"));

    std::string newTopic = message.params[1];
    if (channel->isTopicOpOnly() && !channel->isOperator(client))
        return (sendReply(client, server, ERR_CHANOPRIVSNEEDED, channelName + " :You're not a channel operator"));

    channel->setTopic(newTopic);
    std::string topicMessage = UserPrefix(client) + "TOPIC " + channelName + " :" + newTopic;
    channel->broadcastToChannel(server, topicMessage, client, 0);
}