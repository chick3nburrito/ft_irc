#include "../../include/CommandDispatcher.hpp"

void CommandDispatcher::executeUSER(Client &client, Server &server, const IrcMessage &message)
{
    if(client.getRegistrationStatus())
        return (sendReply(client, server, ERR_ALREADYREGISTRED, ":You may not reregister"));
    if(message.params.size() != 4 || !message.hasTrailing)
        return (sendReply(client, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters"));
    client.setUserName(message.params[0]);
    client.setHostname(message.params[2]);
    client.setRealName(message.params[3]);
    tryRegister(client, server);
}