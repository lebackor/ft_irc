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
#include <algorithm>
#include <map>
#include "user.hpp"
class Channel{
    public:
        Channel(std::string name);
        ~Channel();

        std::string get_channelname();
        std::string get_mode();
        void        set_mode(std::string mode);
        std::string get_topic();
        std::string get_key();
        int     getUsersNb();
        int     get_maxUser();

        void    set_maxUser(int maxUser);
        void        set_topic(std::string topic);
        std::string get_userlistinchan();
        void        set_key(std::string key);
        void    rm_user(int fd);
        void    addUser(int fd, User *usr);
        void        set_invited(int fd);

        std::vector<int>    get_invited();
        std::map<int, User*> &getUsers();
        std::map<int, User*> &getChanOps();
        bool isChanop(int fd);
        void addChanops(int fd, User *usr);
        int searchuserbyname(std::string nickname);
        bool    getBotStatus();
        void    setBotStatus();

    private:
        std::string _name;
        std::string _topic;
        std::string _mode;
        std::string _key;
        int         _maxUser;
        std::map<int, User*> _users;
        std::map<int, User*> _chanUserOps;
        std::vector<int> _invited;
        bool    _isBot;
};