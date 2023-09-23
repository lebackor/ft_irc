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
class Channel{
    public:
        Channel(std::string name);
        ~Channel();

        std::string get_channelname();

    private:
        std::string _name;
        std::string _topic;
        std::map<int, User*> _users;
};