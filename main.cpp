#include "server.hpp"

class Server;

int main(int ac, char **av)
{
    if (ac < 2)
    {
        std::cerr << "Not including the port, exiting." << std::endl;
        exit(1);
    }
	Server server;

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

	return (0);
}