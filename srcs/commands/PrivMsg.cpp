
#include "../../include/CommandDispatcher.hpp"

void CommandDispatcher::executePRIVMSG(Client &client, Server &server, const IrcMessage &message)
{
    if(!client.getRegistrationStatus())
        return (sendReply(client, server, ERR_NOTREGISTERED, ":You are not registered"));
    if(message.params.size() < 2)
        return (sendReply(client, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters"));
    std::vector<std::string> targets = splitDelimiter(message.params[0], ',');
    std::string msg = message.params[1];
    for (size_t i = 0; i < targets.size(); i++)
    {
        if(targets[i].empty())
            continue;
        std::string target = targets[i];
        if(target[0] == '#' || target[0] == '&')
        {
            Channel *channel = server.getChannel(target);
            if(!channel)
            {
                sendReply(client, server, ERR_NOSUCHCHANNEL, target + " :No such channel");
                continue;
            }
            if(!channel->isMember(client))
            {
                sendReply(client, server, ERR_CANNOTSENDTOCHAN, target + " :Cannot send to channel");
                continue;
            }
            channel->broadcastToChannel(server, UserPrefix(client) + "PRIVMSG " + target + " :" + msg, client, 1);
        }
        else
        {
            Client *targetClient = server.getClient(target);
            if(!targetClient)
            {
                sendReply(client, server, ERR_NOSUCHNICK, target + " :No such nick");
                continue;
            }
            server.sendToClient(*targetClient, UserPrefix(client) + "PRIVMSG " + target + " :" + msg);
        }
    }
}