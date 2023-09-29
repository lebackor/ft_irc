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
        void    rm_user(int fd);
        void    addUser(int fd, User *usr);
        int     getUsersNb();
        std::map<int, User*> &getUsers();
        std::map<int, User*> &getChanOps();
        std::map<std::string, std::string> &getBanlist();
        bool isChanop(int fd);
        bool isBan(std::string nickname);
        void addChanops(int fd, User *usr);
        int searchuserbyname(std::string nickname);

    private:
        std::string _name;
        std::string _topic;
        std::map<int, User*> _users;
        std::map<int, User*> _chanUserOps;
        std::map<std::string, std::string>  _banlist;
};