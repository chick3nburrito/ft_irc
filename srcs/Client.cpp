#include "../include/Client.hpp"

Client::Client(): _fd(-1),
		_nickname(""),
		_username(""),
		_realname(""),
		_bufpass(""),
		_registered(false),
		_sendBuffer(""),
		_receiveBuffer("")
 		{}

Client::Client(int fd) : _fd(fd),
		_nickname(""),
		_username(""),
		_realname(""),
		_bufpass(""),
		_registered(false),
		_sendBuffer(""),
		_receiveBuffer("") {}

Client::Client(const Client &other) : _fd(other._fd), _nickname(other._nickname),
		_username(other._username), _realname(other._realname), _bufpass(other._bufpass),
		_registered(other._registered),
		_sendBuffer(other._sendBuffer),
		_receiveBuffer(other._receiveBuffer) 
		{}

Client &Client::operator=(const Client &other)
		{
    		if(this != &other)
    		{
        		_fd = other._fd;
        		_nickname=other._nickname;
        		_username=other._username;
        		_realname=other._realname;
        		_bufpass=other._bufpass;
        		_registered=other._registered;
    		}
			return (*this);
		}

Client::~Client() {}

void Client::setNickName(const std::string &nickname)
{
    _nickname = nickname;
}
void Client::setUserName(const std::string &username)
{
    _username = username;
}
void Client::setRealName(const std::string &realname)
{
    _realname = realname;
}

void Client::setPassword(const std::string &password)
{
    _bufpass = password;
}

const std::string &Client::getNickName() const
{
    return (_nickname);
}
const std::string &Client::getUserName() const
{
    return (_username);
}

const std::string &Client::getRealName() const
{
    return (_realname);
}

const std::string &Client::getPassword() const
{
    return (_bufpass);
}

int Client::getClientFd() const
{
    return (_fd);
}

void Client::setClientFd(int fd)
{
    _fd = fd;
}

void Client::setRegistered()
{
    _registered = true;
}

bool Client::getRegistrationStatus() const
{
    return (_registered);
}

std::string& Client::getSendBuffer()
{
	return _sendBuffer;
}

std::string& Client::getReceiveBuffer()
{
	return _receiveBuffer;
}

void Client::setHostname(const std::string &hostname)
{
	_hostname = hostname;
}

const std::string &Client::getHostname() const
{
	return _hostname;
}
