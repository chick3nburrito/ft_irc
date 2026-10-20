
#include "include/Ircserv.hpp"
#include <csignal>

volatile sig_atomic_t is_running = 1;

void signalHandler(int signal)
{
	std::cout << "\n";
	(void)signal;
	is_running = 0;
}

int main(int ac, char **av)
{
	if (ac < 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
		return 1;
	}

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	Server server(av[1], av[2]);

	if (server.init() != 0)
	{
		std::cerr << "Initialization failed." << std::endl;
		return 1;
	}

	if (server.run() != 0)
	{
		std::cerr << "Execution failed." << std::endl;
		return 1;
	}

	return 0;
}
