
#include "Bot.hpp"

static void sendLine(int fd, const std::string& line)
{
	size_t i = 0;

	while (i < line.size())
	{
		ssize_t sent = send(fd, line.c_str() + i, line.size() - i, 0);
		if (sent < 0)
		{
			std::cerr << strerror(errno) << std::endl;;
			break;
		}
		if (sent == 0)
			break;
		i += static_cast<size_t>(sent);
	}
}

static bool startsWith(const std::string& str, const std::string& prefix)
{
	if (prefix.size() > str.size())
		return false;
	return str.substr(0, prefix.size()) == prefix;
}

static std::string trimCRLF(const std::string& str)
{
	std::string result = str;
	while (!result.empty() && (result[result.size() - 1] == '\n' || result[result.size() - 1] == '\r'))
		result.erase(result.size() - 1, 1);
	return result;
}

static void handleIrcMessage(int fd, const std::string& rawLine)
{
	std::string line = trimCRLF(rawLine);
	if (line.empty())
		return;

	IrcMessage msg = Parser::parse(line);
	if (msg.command == "PRIVMSG" && msg.params.size() >= 2)
	{
		std::string target = msg.prefix.substr(0, msg.prefix.find("!"));
		std::string content = msg.params[1];

		if (startsWith(content, "!help"))
		{
			std::string reply = "PRIVMSG " + target + " :Available commands: !help, !Wshh, !anonym, !time, /bot nickname message\r\n";
			sendLine(fd, reply);
			return;
		}

		if (startsWith(content, "!Wshh"))
		{
			std::string reply = "PRIVMSG " + target + " :Helloooooo from bot!\r\n";
			sendLine(fd, reply);
			return;
		}

		if (startsWith(content, "!time"))
		{
			std::string reply = "PRIVMSG " + target + " :I am a bot, not a clock.\r\n";
			sendLine(fd, reply);
			return;
		}

		std::vector<std::string> anonymMsg;
		if (startsWith(content, "!anonym"))
		{
			anonymMsg = splitDelimiter(content, ' ');
			if (anonymMsg.size() < 3)
			{
				std::string reply = "PRIVMSG " + target + " :Usage: !anonym <nickname> <message>\r\n";
				sendLine(fd, reply);
				return;
			}
			std::string receiver = anonymMsg[1];
			std::string message = content.substr(content.find(receiver) + receiver.length() + 1);
			std::string reply = "PRIVMSG " + receiver + " :" + message + " (anonymous)\r\n";
			sendLine(fd, reply);
			return;
		}
	}
}

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./bot <port> <password>" << std::endl;
		return 1;
	}

	int port = std::atoi(av[1]);
	std::string host = HOST;
	std::string password = av[2];

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
		return 1;
	}

	struct sockaddr_in saddr;
	std::memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(host.c_str());

	if (connect(fd, (struct sockaddr*)&saddr, sizeof(saddr)) == -1)
	{
		std::cerr << "connect() failed" << std::endl;
		close(fd);
		return 1;
	}

	sendLine(fd, "PASS " + password + "\r\n");
	sendLine(fd, "NICK bot\r\n");
	sendLine(fd, "USER bot 0 * :Bot User\r\n");

	char buffer[512];
	while (true)
	{
		ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead <= 0)
			break;

		buffer[bytesRead] = '\0';
		std::string data(buffer);
		std::size_t pos = 0;

		while (true)
		{
			std::size_t end = data.find("\r\n", pos);
			if (end == std::string::npos)
				break;

			std::string line = data.substr(pos, end - pos);
			std::cout << "[Bot] " << line << std::endl;
			handleIrcMessage(fd, line);

			pos = end + 2;
		}
	}

	close(fd);
	return 0;
}
