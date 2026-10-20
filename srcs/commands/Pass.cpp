#include "../../include/CommandDispatcher.hpp"

void CommandDispatcher::executePASS(Client &client, Server &server, const IrcMessage &message)
{
    if(client.getRegistrationStatus())
        return (sendReply(client, server, ERR_ALREADYREGISTRED, ":You may not reregister"));
    if(message.params.size() < 1)
        return (sendReply(client, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters"));
    client.setPassword(message.params[0]);
    tryRegister(client, server);
}