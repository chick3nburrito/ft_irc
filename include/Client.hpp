#pragma once

#include <string>

class Client
{
	private:
		int                 _fd;
		std::string         _nickname;
		std::string         _username;
		std::string         _realname;
		std::string         _hostname;
		std::string          _bufpass;
		bool                _registered;
		std::string         _sendBuffer;
		std::string         _receiveBuffer;

	public:
		Client();
		Client(int fd);
		Client(const Client &other);
		Client& operator=(const Client &other);
		~Client();

		void setNickName(const std::string &nickname);
		void setUserName(const std::string &username);
		void setRealName(const std::string &realname);
		void setClientFd(int fd);
		void setPassword(const std::string &password);
		void setHostname(const std::string &hostname);
		
		std::string& getSendBuffer();
		std::string& getReceiveBuffer();

		const std::string &getNickName() const;
		const std::string &getUserName() const;
		const std::string &getRealName() const;
		const std::string &getPassword() const;
		int                getClientFd() const;
		const std::string &getHostname() const;

		void setRegistered();
		bool getRegistrationStatus() const;
};
