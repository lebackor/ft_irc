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
#include <set>

class User{
    public:
        User(std::string nick, std::string user, std::string host, std::string realname);
        ~User();
        std::string get_nickname();
        std::string get_username();
        std::string get_hostname();
        std::string get_realname();
        std::string get_mode();
        std::set<std::string> &get_channels();
        void    addchannel(std::string name);

        size_t ls_channel();

        void    setnickname(std::string name);
        void    setuser(std::string name);
        void    sethost(std::string name);
        void    setrealname(std::string name);
        void    set_mode(std::string mode);
    private:
        std::string _nick;
        std::string _user;
        std::string _host;
        std::string _realname;
        std::string _mode;
        std::set<std::string> _channels;

};