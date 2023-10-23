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
#include "bot.hpp"
#include <set>
#include <arpa/inet.h>
#include <csignal>
#define MAX_CLIENTS 100


#define ver "1.0"
#define USER_MODE ""
#define CHANNEL_MODE "itkol"

#define OPERPWD "5630"

#define RPL_WELCOME(nick, user, host) \
    (":Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host)

#define RPL_YOURHOST(hostname) \
    ("Your host is " + hostname + ", running version " ver)

#define RPL_TOPIC(channel, topic) \
    (channel + " :" + topic)

#define RPL_CREATED \
("This server was created 09/09/23")

#define RPL_BOUNCE(servername, port) \
    ("Try server " + servername + ", port " + port)

#define ERR_NEEDMOREPARAMS(command) \
    (command + " :Not enough parameters")

#define ERR_NOTONCHANNEL(channel) \
    (channel + " :You're not on that channel")

#define RPL_CHANNELMODEIS(channel, modes) \
    (channel + " " + modes)

#define ERR_TOOMANYCHANNELS(channel) \
    (channel + " :You have joined too many channels")

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

#define ERR_NOSUCHNICK(nick) \
    (nick + " :No such nick/channel")

#define ERR_CHANOPRIVSNEEDED(channel) \
    (channel + " :You're not channel operator")

#define ERR_RESTRICTED \
    (":Your connection is restricted!")

#define ERR_BADCHANNELKEY(channel) \
    (channel + " :Cannot join channel (+k)")

#define ERR_CHANNELISFULL(channel) \
    (channel + " :Cannot join channel (+l)")

#define RPL_YOUREOPER \
    (":You are now an IRC operator")

//473
#define ERR_INVITEONLYCHAN(channel) \
    (channel + " :Cannot join channel (+i)")

#define ERR_PASSWDMISMATCH \
    (":Password incorrect")

//467
#define ERR_KEYSET(channel)\
    (channel + " :Channel key already set")

//472
#define ERR_UNKNOWNMODE(c, channel) \
    (c + ":is unknown mode char to me for " + channel)
//341
#define RPL_INVITING(channel, nick) \
    (channel + " " +nick)

//443
#define ERR_USERONCHANNEL(user, channel) \
    (user + " " + channel + ":is already on channel")

class Bot;

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
        void        setUsers(int fd, User *user);
        void        setChannels(std::string name, Channel *channel);
        void        setBufferSd(int sd, std::string buffer);
        int        get_newClientSocket();
        bool    nicknameAlreadyUse(std::string nick);
        void    sendtoeveryone(std::string message, Channel *chan);

        void    join_command(std::string tmp, int i);
        void    part_command(std::string tmp, int i);
        void    pong_command(std::string tmp, int i);
        void    priv_msg(std::string buffer, int fd);
        void    mode_channel_handler(std::string buffer, int fd);
        void    channel_mode(Channel *channel, std::string mode, int fd, std::string buffer);

        void    invite_command(std::string buffer, int fd);
        void    mode_o_command(Channel *channel, std::string mode, std::string tmp, int fd);
        void    mode_k_command(Channel *channel, std::string mode, std::string buffer, int fd);
        void    mode_l_command(Channel *channel, std::string mode, std::string buffer, int fd);

        void    oper_command(std::string buffer, int fd);
        void    nick_command(std::string buffer, int fd);
        void topic_command(std::string buffer, int sd);
        void kick_command(std::string buffer, int i);
        void sendinchanexceptuser(std::string message, Channel *chan, int sd);
        int searchUserby_nickname(std::string nickname);

        bool    isThereanUser(int fd);
        void    check_connection();
        User*	find_user_byfd(int fd);

        User*    find_user(int fd);

        std::map<int, User*> &getUsers();
        std::map<std::string, Channel*> &getChannels();
        void    setServername(std::string servername);
        std::string    getServername();
        std::string     getPort();
        std::map<int, std::string> &getBufferSd();
        void        set_bufferstr(std::string str);
        std::string get_bufferstr();
        void user_disconnect(int sd);
        Bot *getBot();
        Server(std::string port, std::string password);
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
        std::map<int, std::string> _buffer_sd;
        std::string _serverName;
        std::string _port;
        std::string _password;
        std::string _newbuff;
        std::string _strbuffer;
        Bot         *_ircbot;

        std::string _welcolmeirssi(int code);
        void _sendMessage(std::string message, int sd);

};




std::string send_codes(int code, Server *serv, User *usr, std::string buf1, std::string buf2);
std::string print_user(User *usr);
void removeSpaces(std::string &str);
bool nicknameIsValid(std::string nick);
bool checkInvalidCharacter(char c);
bool channelNameInvalid(std::string name);
bool running_status();
