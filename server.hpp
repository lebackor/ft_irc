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

#define MAX_CLIENTS 100

#define SERVER_NAME "localhost"
#define ver "1.0"
#define USER_MODE "ior"
#define CHANNEL_MODE "ovamntlkb"

//001
#define RPL_WELCOME(nick, user, host) \
    (":Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host)

//002
#define RPL_YOURHOST \
    ("Your host is " SERVER_NAME ", running version " ver)

//003
#define RPL_CREATED \
    ("This server was created 03/04/02")



#define RPL_MYINFO \
    (SERVER_NAME " " ver " " USER_MODE " " CHANNEL_MODE)

void ft_bzero(void* s, std::size_t n);
void ft_bcopy(const void* src, void* dest, std::size_t n);
std::vector<char*> ft_split(const char* input, char delimiter);
std::string welcolme_irssi(int code);
void sendMessage(std::string message, int sd);