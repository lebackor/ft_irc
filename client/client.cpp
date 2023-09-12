#include "../server.hpp"
#include <netdb.h>


void    error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int ac, char **av)
{
    int sockfd, portnb, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[255];
    if (ac < 3)
    {
        std::cerr << "usage " << av[0] << " hostname port" << std::endl;
        exit(0);
    }

    portnb = atoi(av[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening socket");

    server = gethostbyname(av[1]);
    if (server == NULL)
    {
        std::cerr << "Error no such host " << std::endl;
        exit(0);
    }
    ft_bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    ft_bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portnb);    
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Connection failed");
    }
    
    while (1)
    {
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("Error on writing.");
        bzero(buffer, 255);
        n = read(sockfd, buffer, 255);
        if (n < 0)
            error("Error on reading.");
        printf("Server= %s", buffer);

        int i = strncmp("Bye", buffer, 3);
        if (i == 0)
            break;
    }
    close(sockfd);
    return (0);
}
