
#include "../../include/CommandDispatcher.hpp"

std::string getKickReason(const IrcMessage &message)
{
    std::string reason = "";
    if (message.params.size() > 2)
    {
        for (size_t i = 2; i < message.params.size(); ++i)
        {
            reason += " " + message.params[i];
        }
    }
    return reason;
}

void executeKickForChannel(Client &client, Server &server, const std::string &channelName, const std::vector<std::string> &targetNicks, const std::string &kickReason)
{
    if (channelName.empty())
    {
        sendReply(client, server, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
        return;
    }
    Channel *channel = server.getChannel(channelName);
    if (!channel)
    {
        sendReply(client, server, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
        return;
    }
    if (!channel->isOperator(client))
    {
        sendReply(client, server, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
        return;
    }
    for (size_t j = 0; j < targetNicks.size(); ++j)
    {
        const std::string &targetNick = targetNicks[j];
        Client *targetClient = server.getClient(targetNick);
        if (!targetClient)
        {
            sendReply(client, server, ERR_NOSUCHNICK, targetNick + " :No such nick");
            continue;
        }
        if (!channel->isMember(*targetClient))
        {
            sendReply(client, server, ERR_USERNOTINCHANNEL, targetNick + " " + channelName + " :They aren't on that channel");
            continue;
        }
        std::string kickMessage = "KICK " + channelName + " " + targetNick + " :" + kickReason;
        channel->broadcastToChannel(server, UserPrefix(client) + kickMessage, client, 0);
        channel->removeMember(*targetClient);
    }
    if (channel->memberCount() == 0)
    {
        server.removeChannel(channelName);
    }
}

void CommandDispatcher::executeKICK(Client &client, Server &server, const IrcMessage &message)
{
    if(!client.getRegistrationStatus())
        return (sendReply(client, server, ERR_NOTREGISTERED, ":You are not registered"));
    if(message.params.size() < 2)
        return (sendReply(client, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters"));
    

    std::vector<std::string> targetNicks = splitDelimiter(message.params[1], ',');
    std::string kickReason = getKickReason(message);

    std::string channelName = message.params[0];
    executeKickForChannel(client, server, channelName, targetNicks, kickReason);
}