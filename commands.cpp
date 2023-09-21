#include "server.hpp"

class Server;

void    Server::pong_command(std::string tmp, int i)
{
    size_t spacepos = tmp.find(" ");
    if (spacepos != std::string::npos) 
    {
        std::string tmp(_buffer);
        std::string prefix = "PONG ";
        std::string result = prefix;
        std::string extracted_part = tmp.substr(spacepos + 1);
        result.append(extracted_part);
        this->_sendMessage(result, this->clientfd[i].fd);
    }
}

void    Server::part_command(std::string tmp, int i)
{(void) tmp;
        /*size_t spacepos = tmp.find(" ");
    if (spacepos != std::string::npos) 
    {
        std::string tmp(_buffer);
        std::string prefix = "PART ";
        std::string result = prefix;
        std::string extracted_part = tmp.substr(spacepos + 1);
        result.append(extracted_part);
        this->_sendMessage(tmp, this->clientfd[i].fd);
    }
    else */
    this->_sendMessage(":fuerza!len@localhost PART #x Bye", this->clientfd[i].fd); // wrote in brut have to adapt for all user
}

void	Server::join_command(std::string tmp, int i)
{
	this->_sendMessage(tmp, this->clientfd[i].fd);

}