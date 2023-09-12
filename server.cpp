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
    int sockfd, newsockfd, portnb, n;
    char buffer[255];

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

	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
	if (newsockfd < 0)
		error("Error on accepting connection");
	std::cout << "New connection successfull!\n";

	while (1)
	{
		ft_bzero(buffer, 255);
		n = read(newsockfd, buffer, 255);
		//n = recv(newsockfd, buffer, strlen(buffer), MSG_WAITALL);
		if (n < 0)
			error("Error on reading");
		printf("Client : %s\n", buffer);
		ft_bzero(buffer, 255);
		fgets(buffer, 255, stdin);
		//n = send(newsockfd, buffer, strlen(buffer), MSG_WAITALL);
		n = write(newsockfd, buffer, strlen(buffer));
		if (n < 0)
			error("Error on writing");
		
		int i = strncmp("Bye", buffer, 3);
		if (i == 0)
			break;
	}

	close(newsockfd);
	close(sockfd);
	return (0);
}