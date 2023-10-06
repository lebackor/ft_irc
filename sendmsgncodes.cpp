#include "server.hpp"

std::string send_codes(int code, Server *serv, User *usr, std::string buf1, std::string buf2)
{
    (void) buf2;
    std::stringstream ss;
    ss << code;
    std::string codestr = ss.str();
    if (code < 10)
        codestr.insert(0, 2, '0');
    else if (code < 100)
        codestr.insert(0, 1, '0');

    std::string ret;

    if (usr)
        ret += ":" + serv->getServername() + " " + codestr + " " + usr->get_nickname() + " ";
    else
        ret += ":" + serv->getServername() + " " + codestr + " " + "*" + " ";
    
    switch (code)
    {
        case 461:
            ret += ERR_NEEDMOREPARAMS(buf1);
			    break;        
        case 403:
            ret += ERR_NOSUCHCHANNEL(buf1);
                break;
        case 441:
            ret += ERR_USERNOTINCHANNEL(buf1, buf2);
                break;
        case 442:
            ret += ERR_NOTONCHANNEL(buf1);
				break;
        case 431:
            ret += ERR_NONICKNAMEGIVEN;
		    	break;
        case 432:
            ret += ERR_ERRONEUSNICKNAME(buf1);
			    break;
        case 433:
            ret += ERR_NICKNAMEINUSE(buf1);
			    break;
        case 331:
            ret += RPL_NOTOPIC(buf1);
		    	break;
        case 353:
            ret += RPL_NAMREPLY(buf1, buf2);
			    break;
        case 366:
            ret += RPL_ENDOFNAMES(buf1);
			    break;   
    }

    return ret;
}


std::string print_user(User *usr)
{
    return (":" + usr->get_nickname() + "!" + usr->get_username() + "@" + usr->get_hostname() + " ");
}

void Server::sendinchanexceptuser(std::string message, Channel *chan, int sd)
{
    for (std::map<int, User *>::iterator it = chan->getUsers().begin(); it != chan->getUsers().end(); it++)
    {
        if (sd != it->first)
            this->_sendMessage(message, it->first);
    }
    for (std::map<int, User *>::iterator it = chan->getChanOps().begin(); it != chan->getChanOps().end(); it++)
    {
        if (sd != it->first)
            this->_sendMessage(message, it->first);
    }

}

void Server::sendtoeveryone(std::string message, Channel *chan)
{
    for (std::map<int, User *>::iterator it = chan->getUsers().begin(); it != chan->getUsers().end(); it++)
        this->_sendMessage(message, it->first);
    for (std::map<int, User *>::iterator it = chan->getChanOps().begin(); it != chan->getChanOps().end(); it++)
        this->_sendMessage(message, it->first);
}