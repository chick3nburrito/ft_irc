#include "../../include/CommandDispatcher.hpp"

void CommandDispatcher::executeNICK(Client &client, Server &server, const IrcMessage &message)
{
    if(message.params.size() < 1)
        return (sendReply(client, server, ERR_NONICKNAMEGIVEN, ":No nickname given"));
    if(!IsNicknameValid(message.params[0]))
        return (sendReply(client, server, ERR_ERRONEUSNICKNAME, message.params[0] + " :Erroneous nickname"));
    if(client.getNickName() == message.params[0])
        return ;
    Client *other = server.getClient(message.params[0]);
    if(other)
        return (sendReply(client, server, ERR_NICKNAMEINUSE, message.params[0] + " :Nickname is already in use"));
    if(client.getNickName() != "")
    {
        server.sendToClient(client, UserPrefix(client) + "NICK :" + message.params[0]);
        server.broadcastToAllClientChannels(client, UserPrefix(client) + "NICK :" + message.params[0]);
        client.setNickName(message.params[0]);
        return;
    }
    client.setNickName(message.params[0]);
    tryRegister(client, server);
}