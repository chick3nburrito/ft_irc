
#include "../../include/CommandDispatcher.hpp"


void CommandDispatcher::executeINVITE(Client &sender, Server &server, const IrcMessage &message)
{
	if (!sender.getRegistrationStatus())
	{
		sendReply(sender, server, ERR_NOTREGISTERED, ":You are not registered");
		return;
	}

	if (message.params.size() < 2)
	{
		sendReply(sender, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters");
		return;
	}

	std::string targetNick = message.params[0];
	std::string channelName = message.params[1];

	Client *targetClient = server.getClient(targetNick);
	if (!targetClient)
	{
		sendReply(sender, server, ERR_NOSUCHNICK, targetNick + " :No such nick");
		return;
	}

	Channel *channel = server.getChannel(channelName);
	if (!channel)
	{
		sendReply(sender, server, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
		return;
	}

	if (!channel->isMember(sender))
	{
		sendReply(sender, server, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
		return;
	}

	if (channel->isMember(*targetClient))
	{
		sendReply(sender, server, ERR_USERONCHANNEL, targetNick + " " + channelName + " :They are already on that channel");
		return;
	}

	if (channel->isInviteOnly() && !channel->isOperator(sender))
	{
		sendReply(sender, server, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
		return;
	}

	channel->addInvitation(*targetClient);

	std::string inviteMsg = UserPrefix(sender) + "INVITE " + targetNick + " :" + channelName;
	server.sendToClient(*targetClient, inviteMsg);

	sendReply(sender, server, RPL_INVITING, targetNick + " " + channelName);
}


