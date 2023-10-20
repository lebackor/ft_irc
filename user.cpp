#include "user.hpp"


User::User(std::string nick, std::string user, std::string host, std::string realname)
 : _nick(nick), _user(user), _host(host), _realname(realname)
{

}

User::~User()
{
 //   _channels.clear();
}

void    User::setnickname(std::string name){
    this->_nick = name;
}

void    User::setuser(std::string name){
    this->_user = name;

}

void    User::sethost(std::string name){
    this->_host = name;
}

void    User::setrealname(std::string name){
    this->_realname = name;
}



std::string User::get_nickname(){
    return (this->_nick);
}

std::string User::get_username(){
    return (this->_user);
}

std::string User::get_hostname(){
    return (this->_host);
}

std::string User::get_realname(){
    return (this->_realname);
}

std::set<std::string> &User::get_channels(){
    return (this->_channels);
}

void User::addchannel(std::string name)
{
    if (this->_channels.find(name) == this->_channels.end())
        this->_channels.insert(name);
    std::cout << "channel added: " << name << std::endl;
}


size_t User::ls_channel()
{
    return this->_channels.size();
}

std::string User::get_mode()
{
    return this->_mode;
}

void User::set_mode(std::string mode)
{
    this->_mode = mode;
}