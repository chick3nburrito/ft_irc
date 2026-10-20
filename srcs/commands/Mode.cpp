
#include "../../include/CommandDispatcher.hpp"
#include <iostream>
#include <sstream>
#include <cerrno>
#include <cstdlib>
#include <climits>

void CommandDispatcher::executeMODE(Client &sender, Server &server, const IrcMessage &message)
{
	if (!sender.getRegistrationStatus())
		return (sendReply(sender, server, ERR_NOTREGISTERED, ":You are not registered"));

	if (message.params.size() < 1)
		return (sendReply(sender, server, ERR_NEEDMOREPARAMS, ":Not enough parameters"));

	std::string target = message.params[0];
	if (target.empty() || (target[0] != '#' && target[0] != '&'))
		return;

	Channel *channel = server.getChannel(target);
	if (!channel)
		return (sendReply(sender, server, ERR_NOSUCHCHANNEL, target + " :No such channel"));

	if (message.params.size() == 1)
	{
		std::string modes = "+";
		std::string params = "";
		if (channel->isInviteOnly())
			modes += "i";
		if (channel->isTopicOpOnly())
			modes += "t";
		if (channel->hasKey())
		{
			modes += "k";
			if (channel->isMember(sender))
        		params += " " + channel->getKey();
			else
				params += " ***";
		}
		if (channel->hasLimit())
		{
			modes += "l";
			std::stringstream ss;
			ss << channel->getLimit();
			params += " " + ss.str();
		}
		if (modes == "+")
			modes = "";
		return (sendReply(sender, server, RPL_CHANNELMODEIS, target + " :" + modes + params));
	}

	if (!channel->isOperator(sender))
		return (sendReply(sender, server, ERR_CHANOPRIVSNEEDED, target + " :You are not a channel operator"));

	std::string modeString = message.params[1];
	bool adding = true;
	size_t paramIndx = 2;
	std::string appliedModes = "";
	std::string appliedParams = "";
	char lastSign = '\0';

	for (size_t i = 0; i < modeString.length(); i++)
	{
		char c = modeString[i];
		if (c == '+')
		{
			adding = true;
			continue;
		}
		if (c == '-')
		{
			adding = false;
			continue;
		}

		char sign = adding ? '+' : '-';
		bool success = false;
		std::string currentParam = "";

		if (c == 'i')
		{
			channel->setInviteOnly(adding);
			success = true;
		}
		else if (c == 't')
		{
			channel->setTopicOpOnly(adding);
			success = true;
		}
		else if (c == 'k')
		{
			if (adding)
			{
				if (paramIndx < message.params.size())
				{
					channel->setKey(message.params[paramIndx++]);
					currentParam = channel->getKey();
					success = true;
				}
				else
					sendReply(sender, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters");
			}
			else
			{
				channel->removeKey();
				success = true;
			}
		}
		else if (c == 'l')
		{
			if (adding)
			{
				if (paramIndx < message.params.size())
				{
					char *end = NULL;
					errno = 0;
					const std::string &value = message.params[paramIndx++];
					long limit = std::strtol(value.c_str(), &end, 10);
					if (errno == ERANGE || end == value.c_str() || *end != '\0' || limit <= 0 || limit > INT_MAX)
					{
						sendReply(sender, server, ERR_NEEDMOREPARAMS,
								  message.command + " :Invalid channel limit");
					}
					else
					{
						channel->setLimit(static_cast<int>(limit));
						std::stringstream ss;
						ss << limit;
						currentParam = ss.str();
						success = true;
					}
				}
				else
					sendReply(sender, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters");
			}
			else
			{
				channel->removeLimit();
				success = true;
			}
		}
		else if (c == 'o')
		{
			if (paramIndx < message.params.size())
			{
				std::string nick = message.params[paramIndx++];
				Client *targetClient = server.getClient(nick);
				if (targetClient && channel->isMember(*targetClient))
				{
					if (adding)
						channel->addOperator(*targetClient);
					else
						channel->removeOperator(*targetClient);
					currentParam = nick;
					success = true;
				}
				else
					sendReply(sender, server, ERR_USERNOTINCHANNEL, nick + " " + target + " :They aren't on that channel");
			}
			else
				sendReply(sender, server, ERR_NEEDMOREPARAMS, message.command + " :Not enough parameters");
		}
		else
		{
			sendReply(sender, server, ERR_UNKNOWNMODE, std::string(1, c) + " :is unknown mode char to me");
		}

		if (success)
		{
			if (lastSign != sign)
			{
				appliedModes += sign;
				lastSign = sign;
			}
			appliedModes += c;
			if (!currentParam.empty())
				appliedParams += " " + currentParam;
		}
	}

	if (!appliedModes.empty())
	{
		std::string broadcastMsg = UserPrefix(sender) + "MODE " + target + " " + appliedModes + appliedParams;
		channel->broadcastToChannel(server, broadcastMsg, sender, 0);
	}
}
