#include "../include/Replies.hpp"

std::string ServerPrefix(const std::string &replycode, const std::string &nickname, const std::string &servername)
{
    std::stringstream ss;

    ss << ":" << servername
       << " " << replycode
       << " " << (nickname == "" ? "*" : nickname)
       << " ";

    return (ss.str());
}

std::string UserPrefix(const Client &client)
{
    return (":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostname() + " ");
}

void sendReply(Client &client, Server &server, const std::string &replycode, const std::string &reply)
{
    std::string message = ServerPrefix(replycode, client.getNickName()) + reply;
    server.sendToClient(client, message);
}

