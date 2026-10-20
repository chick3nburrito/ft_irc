
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

# include "Client.hpp"
# include "Server.hpp"
# include "Replies.hpp"
# include <string>
# include <map>
# include <cstddef>

class Server;

class Channel
{
private:
	std::string	_name;
	std::string	_topic;
	std::string	_key;

	std::map<int, Client*>	_members;
	std::map<int, Client*>	_operators;
	std::map<int, Client*>	_invited;

	bool	_inviteOnly;
	bool	_topicOpOnly;
	size_t	_limit;

	Channel();
public:
	Channel(const std::string& name);
	Channel(const Channel& other);
	Channel& operator=(const Channel& other);
	~Channel();

	const std::string&	getName() const;
	const std::string&	getTopic() const;
	const std::string&	getKey() const;

	bool				isInviteOnly() const;
	bool				isTopicOpOnly() const;
	bool				hasKey() const;
	bool				hasLimit() const;
	size_t				getLimit() const;

	size_t				memberCount() const;

	bool				isMember(const Client& client) const;
	bool				isOperator(const Client& client) const;
	bool				isInvited(const Client& client) const;

	void	setTopic(const std::string& topic);
	void	setKey(const std::string& key);
	void	removeKey();

	void	setInviteOnly(bool value);
	void	setTopicOpOnly(bool value);

	void	setLimit(size_t limit);
	void	removeLimit();

	bool	addMember(Client& client);
	bool	removeMember(Client& client);

	bool	addOperator(Client& client);
	bool	removeOperator(Client& client);

	bool	addInvitation(Client& client);
	bool	removeInvitation(Client& client);

	bool	isFull() const;
	bool	isKeyValid(const std::string& key) const;

	void	broadcastToChannel(Server& server, const std::string& message, const Client& sender, int flagEx) const;
	void	sendNamesList(Server& server, Client& client) const;
};

#endif

