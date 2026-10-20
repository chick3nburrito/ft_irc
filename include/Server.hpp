#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdlib>
#include <fcntl.h>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>

#include <map>
#include <string>

#include "Client.hpp"
#include "Channel.hpp"


#define MAX_EVENTS 1024
#define RECV_BUFFER_SIZE 512

class Channel;

class Server
{
private:
	int _listenFd, _epollFd;
	int _port;
	std::string _password;
	std::map<int, Client> _clients;
	std::map<std::string, Channel> _channels;

	int accept_clients();
	int disconnect(int client_fd);
	int handle_inputs(Client& client);
	int handle_outputs(Client& client);

public:
	Server(const char *port, const char *pass = "");
	~Server();

	void sendToClient(Client& client, const std::string& str);
	const std::string getPassword() const;
	Client* getClient(const std::string& nick);
	Channel* getChannel(const std::string& name);
	void addChannel(const std::string& name);
	void removeChannel(const std::string& name);
	void broadcastToAllClientChannels(Client& sender, const std::string& message);

	int init();
	int run();
	int shutdown();
	
};

#endif
