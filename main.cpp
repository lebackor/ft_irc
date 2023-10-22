#include "server.hpp"

bool isRunning = true;

class Server;
void	handling_server_msg(Server *server, int i);

int parse2(std::string str)
{
	if (str.size() != 4)
		return 1;
	for (int i = 0;str[i]; i++)
		if (isdigit(str[i]) == 0)
			return 1;
	return 0;
}

int parse1(char *av1)
{
	return 0;
	if (strlen(av1) != 4)
		return 1;
	int port = static_cast<int>(std::strtoul(av1, NULL, 0));
	if ((port >= 6660 && port <= 6669) || port == 6679 || port == 6697)
		return 0;
	return 1;
}

bool running_status()
{
	if (isRunning == true)
		return true;
	else
		return false;
}

void    handler(int signum)
{
	(void)signum;
	isRunning = false;
}

void	handling_server_msg(Server *server, int i)
{
	std::string commandBuffer;
	char buffer[1024];
	int bytesRead;

	bytesRead = recv(server->clientfd[i].fd, buffer, sizeof(buffer), 0);

	if (bytesRead > 0)
		buffer[bytesRead] = '\0';
	else
	{
		std::cout << "Client " << i << " has been disconnected" << std::endl;
		server->user_disconnect(server->clientfd[i].fd);
		memset(&server->clientfd[i], 0, sizeof(server->clientfd[i]));
		
	}
	commandBuffer = buffer;
	if (!commandBuffer.empty() && commandBuffer.find('\n') == std::string::npos && isRunning == true)
	{
		server->getBufferSd().find(server->clientfd[i].fd)->second += buffer;
	}
	else if (commandBuffer.find('\n') != std::string::npos && isRunning == true)
	{
		if (!server->getBufferSd().find(server->clientfd[i].fd)->second.empty())
		{
			server->set_bufferstr(server->getBufferSd().find(server->clientfd[i].fd)->second + commandBuffer);
			server->recvClientMsg(i);
			server->get_bufferstr().clear();
			server->getBufferSd().find(server->clientfd[i].fd)->second.clear();
		}
		else
		{
			server->set_bufferstr(commandBuffer);
			server->recvClientMsg(i);
			server->get_bufferstr().clear();
		}
	}

}


int main(int ac, char **av)
{
	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		if (parse1(av[1]) == 1)
			throw std::runtime_error("Wrong port.");
		if (parse2(av[2]) == 1)
			throw std::runtime_error("Wrong password.");
		Server server(av[1], av[2]);
		server.av = av;
		server.setserversocket();
		while (isRunning == true)
		{
    		std::signal(SIGINT, handler);
			for (int i = 1; i <= MAX_CLIENTS; i++)
			{
				int numReady = poll(server.clientfd, MAX_CLIENTS + 1, -1);

				if (numReady == -1)
					break ;

				if (server.clientfd[0].revents & POLLIN)
					server.acceptClientconnexion();

				if (server.clientfd[i].revents & POLLIN)
					handling_server_msg(&server, i);
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return (0);
}