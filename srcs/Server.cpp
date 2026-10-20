
#include "../include/Ircserv.hpp"
#include "../include/Server.hpp"
#include "../include/CommandDispatcher.hpp"
#include <cstddef>

Server::Server(const char *port, const char *pass)
			: _listenFd(-1), _epollFd(-1), _port(std::atoi(port)),
				_password(pass) {}

Server::~Server() {
	for (std::map<int, Client>::iterator it = _clients.begin();
				it != _clients.end(); ++it) {
		close(it->first);
	}
	_clients.clear();
	if (_listenFd != -1)
		close(_listenFd);
	if (_epollFd != -1)
		close(_epollFd);
}

void Server::sendToClient(Client& client, const std::string& str)
{
	client.getSendBuffer().append(str + "\r\n");

	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.data.fd = client.getClientFd();
	ev.events = EPOLLIN | EPOLLOUT;
	epoll_ctl(_epollFd, EPOLL_CTL_MOD, client.getClientFd(), &ev);

}

const std::string Server::getPassword() const
{
	return _password;
}

Client* Server::getClient(const std::string& nick)
{
	for (std::map<int, Client>::iterator it = _clients.begin(); 
		it != _clients.end(); ++it) {
		if (it->second.getNickName() == nick)
		return &(it->second);
	}
	return NULL;
}

Channel* Server::getChannel(const std::string& name)
{
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); 
		it != _channels.end(); ++it) {
		if (it->second.getName() == name)
		return &(it->second);
	}
	return NULL;
}

void Server::addChannel(const std::string& name)
{
	if(_channels.find(name) == _channels.end())
	{
		_channels.insert(std::make_pair(name, Channel(name)));
	}
}

void Server::removeChannel(const std::string& name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(name);
	if (it != _channels.end())
	{
		_channels.erase(it);
	}
}

void Server::broadcastToAllClientChannels(Client& sender, const std::string& message)
{
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); 
		it != _channels.end(); ++it) 
	{
		if (it->second.isMember(sender))
			it->second.broadcastToChannel(*this, message, sender, 1);
	}
}


int Server::init()
{
	errno = 0;

	_listenFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_listenFd == -1) {
		std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
		return -1;
	}

	int opt = 1;
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
			-1) {
		std::cerr << "Error setting SO_REUSEADDR: " << strerror(errno) << std::endl;
		close(_listenFd);
		return -1;
	}

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(_port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_listenFd, reinterpret_cast<struct sockaddr *>(&saddr),
					 sizeof(saddr)) == -1) {
		std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
		close(_listenFd);
		return -1;
	}

	if (listen(_listenFd, 1024) == -1) {
		std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
		close(_listenFd);
		return -1;
	}

	_epollFd = epoll_create1(0);
	if (_epollFd == -1) {
		std::cerr << "Error creating epoll instance: " << strerror(errno)
							<< std::endl;
		close(_listenFd);
		return -1;
	}

	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.data.fd = _listenFd;
	event.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _listenFd, &event) == -1) {
		std::cerr << "Error adding listenFd to epoll: " << strerror(errno)
							<< std::endl;
		close(_listenFd);
		close(_epollFd);
		return -1;
	}

	return 0;
}

int Server::accept_clients() 
{
	int fd;

	while ((fd = accept(_listenFd, NULL, NULL)) != -1)
	{

		_clients[fd] = Client(fd);

		struct epoll_event eevent;
		memset(&eevent, 0, sizeof(eevent));

		eevent.data.fd = fd;
		eevent.events = EPOLLIN;

		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &eevent) == -1)
		{
			close(fd);
			_clients.erase(fd);
		}
	}

	if (fd == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
	{
		std::cerr << "accept error: " << strerror(errno) << std::endl;
	}
	return 0;
}

int Server::disconnect(int client_fd) 
{
	std::vector<std::string> channelsToRemove;
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); 
		it != _channels.end(); ++it) 
	{
		if (it->second.isMember(_clients[client_fd]))
		{
			it->second.removeMember(_clients[client_fd]);
			if (it->second.memberCount() == 0)
			{
				channelsToRemove.push_back(it->second.getName());
			}
		}
	}
	size_t numChannels = channelsToRemove.size();
	for (size_t i = 0; i < numChannels; ++i)
	{
		removeChannel(channelsToRemove[i]);
	}

	epoll_ctl(_epollFd, EPOLL_CTL_DEL, client_fd, NULL);
	close(client_fd);
	_clients.erase(client_fd);
	return 0;

}

int Server::handle_inputs(Client& client)
{
	char buff[RECV_BUFFER_SIZE];

	ssize_t bytesRead = recv(
		client.getClientFd(),
		buff,
		sizeof(buff),
		MSG_DONTWAIT
	);

	if (bytesRead < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return 0;

		disconnect(client.getClientFd());
		return 0;
	}

	if (bytesRead == 0)
	{
		disconnect(client.getClientFd());
		return 0;
	}

	client.getReceiveBuffer().append(buff, bytesRead);

	std::string& buffer = client.getReceiveBuffer();
	size_t pos;

	while ((pos = buffer.find("\r\n")) != std::string::npos)
	{
		std::string line = buffer.substr(0, pos);

		buffer.erase(0, pos + 2);

		std::cout << "[Client "
		          << client.getClientFd()
		          << "]: "
		          << line
		          << std::endl;

		if (!line.empty())
		{
			IrcMessage msg = Parser::parse(line);
			if (!msg.command.empty())
				CommandDispatcher().execute(client, *this, msg);
		}
	}

	if (!client.getSendBuffer().empty())
	{
		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));

		ev.data.fd = client.getClientFd();
		ev.events = EPOLLIN | EPOLLOUT;

		epoll_ctl(
			_epollFd,
			EPOLL_CTL_MOD,
			client.getClientFd(),
			&ev
		);
	}

	return 0;
}

int Server::handle_outputs(Client& client) 
{
	if (!client.getSendBuffer().empty())
	{
		ssize_t bytesSent = send(client.getClientFd(),
			client.getSendBuffer().c_str(),
			client.getSendBuffer().length(),
			MSG_DONTWAIT | MSG_NOSIGNAL
		);
		if (bytesSent < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				disconnect(client.getClientFd());
			}
			return 0;
		}
		if (bytesSent > 0)
			client.getSendBuffer().erase(0, bytesSent);
	}
	if (client.getSendBuffer().empty())
	{
		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		ev.data.fd = client.getClientFd();
		ev.events = EPOLLIN;
		epoll_ctl(_epollFd, EPOLL_CTL_MOD, client.getClientFd(), &ev);
	}
	return 0;
}

int Server::run()
{
	std::cout << "LOG: Server is running on port " << _port << std::endl;
	int n = 0;
	struct epoll_event events[MAX_EVENTS];

	errno = 0;
	while (is_running)
	{
		n = epoll_wait(_epollFd, events, MAX_EVENTS, -1);
		if (n == -1)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "epoll_wait error: " << strerror(errno) << std::endl;
			return -1;
		}
		for (int i = 0; i < n; i++)
		{
			int client_fd = events[i].data.fd;
			
			if (client_fd == _listenFd)
				accept_clients();
			else 
			{
				if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) {
					disconnect(client_fd);
					continue;
				}
				if (_clients.find(client_fd) == _clients.end())
					continue;
				Client& client = _clients[client_fd];
				if ((events[i].events & EPOLLIN))
					handle_inputs(client);
				if (_clients.find(client_fd) == _clients.end())
				{
					continue;
				}
				if ((events[i].events & EPOLLOUT) && handle_outputs(client) == 0)
					continue;
			}
		}
	}
	shutdown();
	return 0;
}


int Server::shutdown()
{

	std::cout << "LOG: shutdown. Bye" << std::endl;
	for (std::map<int, Client>::iterator it = _clients.begin();
		 it != _clients.end(); ++it)
	{
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, it->first, NULL);
		close(it->first);
	}

	_clients.clear();

	if (_listenFd != -1)
	{
		close(_listenFd);
		_listenFd = -1;
	}

	if (_epollFd != -1)
	{
		close(_epollFd);
		_epollFd = -1;
	}
	return 0;
}
