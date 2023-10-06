#include "channel.hpp"


Channel::Channel(std::string name) :_name(name){}

Channel::~Channel(){}

std::string Channel::get_channelname()
{
    return (_name);
}

void    Channel::rm_user(int fd)
{
    std::map<int, User*>::iterator it;

    if ((it = this->_users.find(fd)) != this->_users.end())
        this->_users.erase(it);
    else if ((it = this->_chanUserOps.find(fd)) != this->_chanUserOps.end())
        this->_chanUserOps.erase(it);
}

int Channel::getUsersNb() 
{
    return (this->_users.size());
}

std::map<int, User*> & Channel::getUsers()
{
    return this->_users;
}

std::map<int, User*> & Channel::getChanOps()
{
    return this->_chanUserOps;
}

std::string Channel::get_topic(){
    return this->_topic;
}

void        Channel::set_topic(std::string topic){
    this->_topic = topic;
}


void Channel::addUser(int fd, User *usr)
{
    this->_users.insert(std::make_pair(fd, usr));
}

void Channel::addChanops(int fd, User *usr)
{
    this->_chanUserOps.insert(std::make_pair(fd, usr));
}


bool Channel::isBan(std::string nickname)
{
    if (this->_banlist.find(nickname) != this->_banlist.end())
        return (true);
    return (false);
}

bool    Channel::isChanop(int fd)
{
    if (this->_chanUserOps.find(fd) != this->_chanUserOps.end())
        return (true);
    return (false);
}

std::string Channel::get_userlistinchan()
{
    std::string output;
    for (std::map<int, User *>::iterator it = this->_chanUserOps.begin(); it != this->_chanUserOps.end(); it++)
    {
        if (!output.empty())
            output += " ";
        output += "@";
        output += it->second->get_nickname();
    }
    for (std::map<int, User *>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (!output.empty())
            output += " ";
        output += it->second->get_nickname();
    }
    return (output);
}


int Channel::searchuserbyname(std::string nickname)
{
    for (std::map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (nickname.compare(it->second->get_nickname()) == 0)
            return it->first;
    }
    for (std::map<int, User*>::iterator it = this->_chanUserOps.begin(); it != this->_chanUserOps.end(); it++)
    {
        if (nickname.compare(it->second->get_nickname()) == 0)
            return it->first;
    }
    return (-1);
}

std::string Channel::get_mode(){
    return this->_mode;
}


void        Channel::set_mode(std::string mode){
    size_t pos = this->_mode.find(mode);
    if (pos != std::string::npos)
    {
        this->_mode.erase(pos, mode.length());
    }
    else
        this->_mode += mode; 
}
