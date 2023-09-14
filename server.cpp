#include "server.hpp"

void    error(const char *msg)
{
    perror(msg);
    exit(1);
}

std::string welcolme_irssi(int code)
{
	std::stringstream ss;
	ss << code;
	std::string codestr = ss.str();
    if (code < 10)
        codestr.insert(0, 2, '0');
    else if (code < 100)
	{
        codestr.insert(0, 1, '0');
	}


	std::string ret;
	std::string serv_name = "len ";
    ret += ":" + serv_name+ " " + codestr + " " + "*" + " ";

	std::string nickname, username, hostname;
	nickname = "fuerza";
	username = "len";
	hostname = "len";

	switch (code)
	{
        case 001:
            ret += RPL_WELCOME(nickname, username, hostname);
			break;
		case 002:
            ret += RPL_YOURHOST;
			break;
        case 003:
            ret += RPL_CREATED;
			break;
        case 004:
            ret += RPL_MYINFO;
			break;		
	}
	return ret;
}

void sendMessage(std::string message, int sd)
{
	message += "\r\n";
	if (send(sd, message.c_str(), message.length(), 0) < 0)
		throw std::runtime_error("Error sending message.");
}


int main(int ac, char **av)
{
    if (ac < 2)
    {
        std::cerr << "Not including the port, exiting." << std::endl;
        exit(1);
    }
    int serverSocket, portnb;
    char buffer[1024];
	int newClientSocket = 0;


    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
        error("Error opening socket");

    portnb = atoi(av[1]);

    memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portnb);

	cli_len = sizeof(cli_addr);
    if (bind(serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
    	error("Binding failed.");
	}

	listen(serverSocket, 5); // 5 is the number max of clients can connect simultanately


	struct pollfd fds[MAX_CLIENTS + 1];
	memset(fds, 0, sizeof(fds));

	fds[0].fd = serverSocket;
	fds[0].events = POLLIN;

	int flags = fcntl(serverSocket, F_GETFL);
	fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);
	
	while (1)
	{
		int numReady = poll(fds, MAX_CLIENTS + 1, -1);
		if (numReady == -1)
			perror("Error of poll()");

		if (fds[0].revents & POLLIN)
		{
			newClientSocket = accept(serverSocket, (struct sockaddr*)&cli_addr, &cli_len);	
			if (newClientSocket == -1)
				perror("Error accepting a connection");
			else
			{
				std::cout << "Server has detected a connection !" << std::endl;
				int clientFlags = fcntl(newClientSocket, F_GETFL, 0);
				fcntl(newClientSocket, F_SETFL, clientFlags | O_NONBLOCK);

				sendMessage(welcolme_irssi(001), newClientSocket);
				sendMessage(welcolme_irssi(002), newClientSocket);
				sendMessage(welcolme_irssi(003), newClientSocket);
				sendMessage(welcolme_irssi(004), newClientSocket);
			
				for (int i = 1; i <= MAX_CLIENTS; i++)
				{
					if (fds[i].fd == 0)
					{
						fds[i].fd = newClientSocket;
						fds[i].events = POLLIN;


						break;
					}
				}
			}	
		}

		// gerer les donnes des clients connecter la
		for (int i = 1; i <= MAX_CLIENTS; i++)
		{
			if (fds[i].revents & POLLIN)
			{
				int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);

				if (bytesRead <= 0)
				{
					std::cout << "Client " << i << " has been disconnected" << std::endl;
					close(fds[i].fd);
					memset(&fds[i], 0, sizeof(fds[i]));
				}
				else
				{
					buffer[bytesRead] = '\0';
					//je dois trouver un moyen de split buffer en 2 pour recup en index 0 la commande puis en index 1 le but(join, part, nick etc) if ()
					std::cout << "Client " << i << ": " << buffer << std::endl;
				}
			}
		}
	
	}

	close(serverSocket);
	return (0);
}