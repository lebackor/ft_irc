#include "user.hpp"


User::User(std::string nick, std::string user, std::string host, std::string realname)
 : _nick(nick), _user(user), _host(host), _realname(realname)
{
  /*  std::cout << "From User class : " << std::endl;
    std::cout << "Nickname: " << _nick << std::endl;
    std::cout << "Username: " << _user << std::endl;
    std::cout << "Hostname: " << _host << std::endl;
    std::cout << "Realname: " << _realname << std::endl;
*/
}

User::~User()
{

}

void    User::_setnickname(std::string name){
    this->_nick = name;
}

void    User::_setuser(std::string name){
    this->_user = name;

}

void    User::_sethost(std::string name){
    this->_host = name;
}

void    User::_setrealname(std::string name){
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