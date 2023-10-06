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
{
    std::string tmp1(tmp);
    std::string channelname = "";
    size_t q;

    
    if ((q = tmp1.find_first_not_of(" \t\r\n", 5)) != std::string::npos)
        channelname = tmp1.substr(q, (tmp1.find_first_of(" \t\r\n", i) - q));
    if (channelname.empty())
    {
        _sendMessage(send_codes(461, this, find_user(i), "PART", ""), this->clientfd[i].fd);
        return ;
    }

    std::string msg = "";

    size_t j = 1;

    j = tmp1.find_first_of(" \t\r\n", j);
    j = tmp1.find_first_not_of(" \t\r\n", j);
    
    int u = j;

    if ((j = tmp1.find_first_of("\r\n", j)) != std::string::npos)
        msg = tmp1.substr(u, (tmp1.find_first_of("\r\n", u) - u));
    int nbchannels = 1 + std::count(channelname.begin(), channelname.end(), ',');

    for (int t = 0; t < nbchannels; t++)
    {
        std::string chan_name = channelname.substr(0, channelname.find(","));
        channelname.erase(0, channelname.find(",") + 1);
        if (this->getChannels().find(chan_name) == this->getChannels().end())
            _sendMessage(send_codes(403, this, find_user(i), chan_name, ""), this->clientfd[i].fd);
        else if (find_user(i)->get_channels().find(chan_name) == find_user(i)->get_channels().end())
            _sendMessage(send_codes(442, this, find_user(i), chan_name, ""), this->clientfd[i].fd);
        else
        {
            std::string response = print_user(find_user(i));
            response += "PART " + chan_name + " " + msg;
            _sendMessage(response, this->clientfd[i].fd);
            this->getChannels().find(chan_name)->second->rm_user(i); // maybe dégraisser cette ligne avec une fonction ou un define
            if (this->getChannels().find(chan_name)->second->getUsersNb() == 0)
            {
                delete this->getChannels().find(chan_name)->second;
                this->getChannels().erase(chan_name);
            }
            find_user(i)->get_channels().erase(chan_name);
        }
    }
}


void	Server::join_command(std::string buffer, int i)
{
    
    size_t k = 0, j = 0;
    std::string buf(buffer);
    std::string channelsName = "";
    if ((k = buf.find_first_not_of(" \t\r\n", 5)) != std::string::npos)
        channelsName = buf.substr(k, ((j = buf.find_first_of(" \t\r\n", k)) - k));
    if (channelsName.empty())
    {
        this->_sendMessage(send_codes(461, this, find_user(i), "JOIN", ""), this->clientfd[i].fd);
        return;
    }
    int nbOfChannels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');
    std::string keysForChannels = "";
    if (buf.find_first_not_of(" \t\r\n", j) != std::string::npos)
    {
        j = buf.find_first_not_of(" \t\r\n", j);
        keysForChannels = buf.substr(j, (buf.find_first_of(" \t\r\n", j) - j));
    }
    for (int k = 0; k < nbOfChannels; k++)
    {
        std::string channelName = channelsName.substr(0, channelsName.find(","));
        channelsName.erase(0, channelsName.find(",") + 1);
        if (find_user(i)->ls_channel() > 10)
        {
            this->_sendMessage(send_codes(405, this, find_user(i), channelName, ""), this->clientfd[i].fd);
            break;
        }
        if (!channelNameInvalid(channelName))
        {
            this->_sendMessage(channelName + ":Erroneous Channel Name", this->clientfd[i].fd);
            continue;
        }
        std::string key = keysForChannels.substr(0, keysForChannels.find(","));
        keysForChannels.erase(0, keysForChannels.find(",") + 1);
        if (this->getChannels().find(channelName) == this->getChannels().end())
        {
            Channel *chan = new Channel(channelName);
            this->setChannels(channelName, chan);
        }

        if (this->getChannels().find(channelName)->second->getUsersNb() == 0)
            this->getChannels().find(channelName)->second->addChanops(this->clientfd[i].fd, find_user(i));
        else if (this->getChannels().find(channelName)->second->searchuserbyname(find_user(i)->get_nickname()) != -1)
            return;
        else
            this->getChannels().find(channelName)->second->addUser(this->clientfd[i].fd, find_user(i));
        find_user(i)->addchannel(channelName);
        std::string userAnswer = print_user(find_user(i));
        userAnswer += "JOIN " + channelName;
        if (this->getChannels().find(channelName)->second->get_mode().find("a") == std::string::npos)
            sendtoeveryone(userAnswer, this->getChannels().find(channelName)->second);
        if (this->getChannels().find(channelName)->second->get_topic() == "")
            this->_sendMessage(send_codes(331, this, find_user(i), channelName, ""), this->clientfd[i].fd);
        else
            this->_sendMessage(send_codes(332, this, find_user(i), channelName, this->getChannels().find(channelName)->second->get_topic()), this->clientfd[i].fd);
        std::string listOfUser = this->getChannels().find(channelName)->second->get_userlistinchan();
        if (this->getChannels().find(channelName)->second->get_mode().find("a") == std::string::npos)
        {
            this->_sendMessage(send_codes(353, this, find_user(i), channelName, listOfUser), this->clientfd[i].fd);
            this->_sendMessage(send_codes(366, this, find_user(i), channelName, ""), this->clientfd[i].fd);
        }
        if (!this->getChannels().find(channelName)->second->get_mode().empty())
            this->_sendMessage(send_codes(324, this, find_user(i), channelName, this->getChannels().find(channelName)->second->get_mode()), this->clientfd[i].fd);
    }
 /*
   std::string channelname;

    int p = tmp.find(' ');

    channelname = tmp.substr(p + 1);
    
    removeSpaces(channelname);

    
    User *usr = this->_users[clientfd[i].fd];
    if (getChannels().find(channelname) == getChannels().end())
    {
	    Channel* channel = new Channel(channelname);
        setChannels(channelname, channel);
        channel->addUser(clientfd[i].fd, usr);
    }
    else
    {
        Channel *channel = getChannels()[channelname];
        channel->addUser(clientfd[i].fd, usr);
    }
    

    usr->addchannel(channelname);
    
    this->_sendMessage(tmp, this->clientfd[i].fd);


 */
}
/*
void Server::mode_o_command(Channel *channel, std::string mode, std::string tmp, int fd)
{
    int i = 0;
    int j = 0;
    while (tmp[i] && j < 3)
    {
        while (tmp[i] == ' ' || tmp[i] == '\t')
        {
            i++;
        }
        j++;
        i++;
    }
    std::string name = tmp.substr(i, (tmp.find_first_of(" \t\r\n", i) - i));
    int userfd = channel->searchuserbyname(name);
    if (userfd == -1)
    {
        this->_sendMessage(send_codes(441, this, find_user(fd), name, channel->get_channelname()), fd);
        return ;
    }
    channel->rm_user(userfd);
    if (mode[0] == '-')
        channel->addUser(userfd, find_user(userfd));
    else
        channel->addChanops(userfd, find_user(userfd));
    std::string response = print_user((find_user(fd)));
    response += "MODE " + channel->get_channelname() + " " + mode + " " + name;
    this->_sendMessage(response, userfd);

}
*/
void    Server::priv_msg(std::string buffer, int fd)
{
    int tmp;
    std::string buf(buffer);
    if (buf.compare(0, 6, "NOTICE") == 0)
        tmp = 7;
    else
        tmp = 8;
    size_t i;
    std::string msgtarget = "";
    if ((i = buf.find_first_not_of(" \t\r\n", tmp)) != std::string::npos)
        msgtarget = buf.substr(i, (buf.find_first_of(" \t\r\n", i) - i));
    std::string msg = "";
    size_t j;
    j = buf.find_first_of(" \t\r\n", i);
    if ((j = buf.find_first_not_of(" \t\r\n", j)) != std::string::npos)
        msg = buf.substr(j, (buf.find_first_of("\r\n", j) - j));
    std::string idOfChannel = "#&+";
    int userToSendSd;
    std::string userAnswer;
    userAnswer = print_user(find_user(fd));
    userAnswer += "PRIVMSG " + msgtarget + " " + msg;

    removeSpaces(msgtarget);
    if (!msgtarget.empty() && (idOfChannel.find(msgtarget[0]) != std::string::npos))
    {
        if (this->getChannels().find(msgtarget) == this->getChannels().end())
            this->_sendMessage(send_codes(401, this, find_user(fd), msgtarget, ""), this->clientfd[fd].fd);
        else if (this->getChannels().find(msgtarget)->second->isBan(find_user(fd)->get_nickname()) == true)
           this->_sendMessage(send_codes(404, this, find_user(fd), msgtarget, ""), this->clientfd[fd].fd);
        else
            this->sendinchanexceptuser(userAnswer, this->getChannels().find(msgtarget)->second, this->clientfd[fd].fd);
    }
    else
    {
        if ((userToSendSd = this->searchUserby_nickname(msgtarget)) == -1)
            this->_sendMessage(send_codes(401, this, find_user(fd), msgtarget, ""), this->clientfd[fd].fd);
        else
            this->_sendMessage(userAnswer, userToSendSd);       
    }
}

void Server::nick_command(std::string buffer, int fd)
{
    size_t i = 0;
    std::string newNickname = "";
    std::string buf(buffer);
    if ((i = buf.find_first_not_of(" \t\r\n", 5)) != std::string::npos)
        newNickname = buf.substr(i, (buf.find_first_of(" \t\r\n", i) - i));
    if (newNickname.empty())
    {
        this->_sendMessage(send_codes(431, this, find_user(fd), "", ""), this->clientfd[fd].fd);
        return;
    }
   /* if (find_user(fd)->getMode().find('r') != std::string::npos)
    {
        this->_sendMessage(send_codes(484, this, find_user(fd), "", ""), fd);
        return;
    }*/
    if (!nicknameIsValid(newNickname))
    {
        this->_sendMessage(send_codes(432, this, find_user(fd), newNickname, ""), this->clientfd[fd].fd);
        return;
    }
    if (this->nicknameAlreadyUse(newNickname))
    {
        this->_sendMessage(send_codes(433, this, find_user(fd), newNickname, ""), this->clientfd[fd].fd);
        return;
    }
    std::string userAnswer = print_user(find_user(fd));
    userAnswer += "NICK " + newNickname;
    this->_sendMessage(userAnswer, this->clientfd[fd].fd);
    find_user(fd)->setnickname(newNickname);
}