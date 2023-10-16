#include "server.hpp"

class Server;

int checkav2(std::string str)
{
	if (str.size() != 4)
		return 1;
	for (int i = 0;str[i]; i++)
		if (isdigit(str[i]) == 0)
			return 1;
	return 0;
}

int checkav1(char *av1)
{
	return 0;
	if (strlen(av1) != 4)
		return 1;
//	int port = static_cast<int>(std::strtoul(av1, NULL, 0));
//	if ((port >= 6660 && port <= 6669) || port == 6679 || port == 6697)
//		return 0;
	return 1;
}


int main(int ac, char **av)
{
	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		if (checkav1(av[1]) == 1)
			throw std::runtime_error("Wrong port.");
		if (checkav2(av[2]) == 1)
			throw std::runtime_error("Wrong password.");
		Server server(av[1], av[2]);
		server.av = av;
		server.setserversocket();
		while (1)
		{
			for (int i = 1; i <= MAX_CLIENTS; i++)
			{
			
			int numReady = poll(server.clientfd, MAX_CLIENTS + 1, -1);
			if (numReady == -1)
				perror("Error of poll()");

			if (server.clientfd[0].revents & POLLIN) // ce serait mieux de faire boucle i pour check chaque node, ici on perd des ressources pr rien car il entre dedans dans  tt les cas et accept quand il en capte une
				server.acceptClientconnexion();

			// gerer les donnes des clients connecter la
			
				if (server.clientfd[i].revents & POLLIN)
					server.recvClientMsg(i);
			}
		
		}

	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	

	return (0);
}