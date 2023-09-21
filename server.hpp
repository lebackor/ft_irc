#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdexcept>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
#include <map>
#include "user.hpp"


#define MAX_CLIENTS 100

//#define SERVER_NAME "localhost"
#define ver "1.0"
#define USER_MODE "ior"
#define CHANNEL_MODE "ovamntlkb"

//001
#define RPL_WELCOME(nick, user, host) \
    (":Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host)

//002
#define RPL_YOURHOST(hostname) \
    ("Your host is " + hostname + ", running version " ver)

//003
#define RPL_CREATED \
    ("This server was created 03/04/02")



#define RPL_MYINFO(hostname) \
    (hostname + " " ver " " USER_MODE " " CHANNEL_MODE)

class Server{
    public:
        char **av;
        struct pollfd clientfd[MAX_CLIENTS + 1];
        struct pollfd   get_clientfd();
       
       
        void    error(const char *msg);
        void    acceptClientconnexion();
        void    recvClientMsg(int i);
        void    setserversocket();
        bool    firstConnection(int i);
        void	   setUserInfo();
        void        setUsers(int fd, User *user);
        int        get_newClientSocket();
        std::map<int, User*> &getUsers();
        Server();
        ~Server();
    private:


        int    _serverSocket;
        char _buffer[1024];
        int _newClientSocket;
        int _portnb;
        struct sockaddr_in _servAddr, _cliAddr;
        socklen_t _cliLen;
        std::map<int, User*>  _users;
        

        std::string _welcolmeirssi(int code);
        void _sendMessage(std::string message, int sd);

};




void ft_bzero(void* s, std::size_t n);
void ft_bcopy(const void* src, void* dest, std::size_t n);
std::vector<char*> ft_split(char* input, char delimiter);
