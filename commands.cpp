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
    User* usr = find_user(i);

    std::string channelname;
    if (!tmp.empty())
    {
        int p = tmp.find(' ');
        
        channelname = tmp.substr(p + 2);
        this->_sendMessage(":" + usr->get_nickname() + "!" + usr->get_username() + "@" + usr->get_hostname() + " " + "PART #" + channelname, this->clientfd[i].fd);
    }
}


void	Server::join_command(std::string tmp, int i)
{
    std::string channelname;

    
    if (!tmp.empty() && tmp.find(' ') != std::string::npos)
    {
        int p = tmp.find(' ');

        channelname = tmp.substr(p + 2);
    
    }
        
	Channel* channel = new Channel(channelname);
    
    setChannels(channelname, channel);

    User *usr = this->_users[clientfd[i].fd];

    usr->addchannel(channelname);
    
    this->_sendMessage(tmp, this->clientfd[i].fd);

}