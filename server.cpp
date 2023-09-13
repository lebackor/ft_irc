#include "server.hpp"

void    error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int ac, char **av)
{
    if (ac < 2)
    {
        std::cerr << "Not including the port, exiting." << std::endl;
        exit(1);
    }
    int sockfd, portnb, n;
    char buffer[255];
	int num_event = 0;


    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening socket");
    ft_bzero((char *) &serv_addr, sizeof(serv_addr));
    portnb = atoi(av[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portnb);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
    	error("Binding failed.");
	}

	listen(sockfd, 5); // 5 is the number max of clients can connect simultanately

	cli_len = sizeof(cli_addr);

	struct pollfd fds[SOMAXCONN + 1];
	size_t i = 1;
	//int flags = fcntl(sockfd, F_GETFL);
	//fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	while (1)
	{
		//std::cout << "in while\n";
		for (size_t j = 0; j < i; j++)
		{
			//std::cout << "in for\n";
			if (fds[j].revents != POLLIN)
				continue;
			if (fds[j].fd == sockfd)
			{
				std::cout << "in if \n";
				fds[j].revents = 0;
				fds[i].fd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
				if (fds[i].fd < 0)
					error("Error on accepting connection");
				std::cout << "New connection successfull!\n";
				std::cout << "before poll\n";
				num_event = poll(fds, i, -1);
				std::cout << "after poll\n";
			}
			fds[i++].events = POLLIN;

		}
		for (int k = 0; k < num_event; k++)
		{
			std::cout << "for after poll\n";
			if (fds[k].revents == POLLIN)
			{
				std::cout << "for after poll in if\n";
				ft_bzero(buffer, 255);
				n = read(fds[i].fd, buffer, 255);
				if (n < 0)
					error("Error on reading");
				printf("Client : %s\n", buffer);
			}
		}

/*		ft_bzero(buffer, 255);
		fgets(buffer, 255, stdin);
		n = write(newsockfd, buffer, strlen(buffer));
		if (n < 0)
			error("Error on writing");
*/		
		int i = strncmp("Bye", buffer, 3);
		if (i == 0)
			break;
	}

	close(sockfd);
	return (0);
}