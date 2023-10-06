#pragma once

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
#include "channel.hpp"
#include <set>
#define MAX_CLIENTS 100

//#define SERVER_NAME "localhost"
#define ver "1.0"
#define USER_MODE "ior"
#define CHANNEL_MODE "ovamntlkb"


#define RPL_WELCOME(nick, user, host) \
    (":Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host)


#define RPL_YOURHOST(hostname) \
    ("Your host is " + hostname + ", running version " ver)


#define RPL_CREATED \
("This server was created 03/04/02")

#define ERR_NEEDMOREPARAMS(command) \
    (command + " :Not enough parameters")

#define ERR_NOTONCHANNEL(channel) \
    (channel + " :You're not on that channel")

#define ERR_NOSUCHCHANNEL(channel) \
    (channel + " :No such channel")

#define RPL_MYINFO(hostname) \
    (hostname + " " ver " " USER_MODE " " CHANNEL_MODE)

#define ERR_USERNOTINCHANNEL(nick, channel) \
    (nick + " " + channel + " :They aren't on that channel")

#define RPL_NOTOPIC(channel) \
    (channel + " :No topic is set")

#define ERR_NONICKNAMEGIVEN \
    (":No nickname given")

#define RPL_NAMREPLY(channel, listOfUsers) \
    ("= " + channel + " :" + listOfUsers)

#define RPL_ENDOFNAMES(channel) \
    (channel + " :End of NAMES list")

#define ERR_ERRONEUSNICKNAME(nick) \
    (nick +" :Erroneous nickname")

#define ERR_NICKNAMEINUSE(nick) \
    (nick + " :Nickname is already in use")

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
        void        setChannels(std::string name, Channel *channel);
        int        get_newClientSocket();
        bool    nicknameAlreadyUse(std::string nick);
        void    sendtoeveryone(std::string message, Channel *chan);
        
        void    join_command(std::string tmp, int i);
        void    part_command(std::string tmp, int i);
        void    pong_command(std::string tmp, int i);
        void    priv_msg(std::string buffer, int fd);
        void    mode_o_command(Channel *channel, std::string mode, std::string tmp, int fd);
        void    nick_command(std::string buffer, int fd);
        
        void sendinchanexceptuser(std::string message, Channel *chan, int sd);
        int searchUserby_nickname(std::string nickname);


        User*    find_user(int fd);

        std::map<int, User*> &getUsers();
        std::map<std::string, Channel*> &getChannels();
        void    setServername(std::string servername);
        std::string    getServername();
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
        std::map<std::string, Channel*> _channels;
        std::string _serverName;
        

        std::string _welcolmeirssi(int code);
        void _sendMessage(std::string message, int sd);

};




void ft_bzero(void* s, std::size_t n);
void ft_bcopy(const void* src, void* dest, std::size_t n);
std::vector<char*> ft_split(char* input, char delimiter);
std::string send_codes(int code, Server *serv, User *usr, std::string buf1, std::string buf2);
std::string print_user(User *usr);
void removeSpaces(std::string &str);
bool nicknameIsValid(std::string nick);
bool checkInvalidCharacter(char c);
bool channelNameInvalid(std::string name);