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
class User{
    public:
        User(std::string nick, std::string user, std::string host, std::string realname);
        ~User();
        std::string get_nickname();
        std::string get_username();
        std::string get_hostname();
        std::string get_realname();

    private:
        std::string _nick;
        std::string _user;
        std::string _host;
        std::string _realname;
        void    _setnickname(std::string name);
        void    _setuser(std::string name);
        void    _sethost(std::string name);
        void    _setrealname(std::string name);
};