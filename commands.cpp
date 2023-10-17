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

bool    find_mode(char c, std::string buf)
{
    return (buf.find(c) != std::string::npos);
}

void    Server::channel_mode(Channel *channel, std::string mode, int fd, std::string buffer)
{
    std::string channel_mode = channel->get_mode();

    int i;

    if (mode[0] == '-')
    {
        std::string deletedmode;

        for (i = 1; mode[i]; i++)
        {
            if (find_mode(mode[i], CHANNEL_MODE) == false)
            {
                std::string strmode(1, mode[i]);
                this->_sendMessage(send_codes(472, this, find_user(fd), strmode, channel->get_channelname()), this->clientfd[fd].fd);
            }
            else if (find_mode(mode[i], "itkol") == true)
	        {
                //if (mode[i] == i)
               // if (mode[i] == i)
                if (mode[i] == 'k')
                    mode_k_command(channel, mode, buffer, fd);
                if (mode[i] == 'l')
                    mode_l_command(channel, mode, buffer, fd);
                if (mode[i] == 'o')
                    mode_o_command(channel, mode, buffer, fd);
	    	    
                
                if (mode[i] == 'k' || mode[i] == 'l')
			        deletedmode += mode[i];
	        }
	        else if (channel_mode.find(mode[i]) != std::string::npos)
            {
                deletedmode += mode[i];
                channel_mode.erase(channel_mode.find(mode[i]), 1);
            }
        }
        channel->set_mode(channel_mode);
        std::string userAnswer = print_user(find_user(fd));
        if (!deletedmode.empty())
            userAnswer += "MODE " + channel->get_channelname() + " -" + deletedmode;
        if (userAnswer.find("MODE") != std::string::npos)
            this->sendtoeveryone(userAnswer, channel);
    }
    else
    {
        std::string addmode;
        for ((mode[0] != '+') ? i = 0 : i = 1; mode[i]; i++)
        {
            if (find_mode(mode[i], CHANNEL_MODE) == false)
            {
                std::string stringMode(1, mode[i]);
                this->_sendMessage(send_codes(472, this, find_user(fd), stringMode, channel->get_channelname()), this->clientfd[fd].fd);
            }
            else if (find_mode(mode[i], "itkol") == true)
            {
                if (mode[i] == 'k')
                    mode_k_command(channel, mode, buffer, fd);
                if (mode[i] == 'l')
                    mode_l_command(channel, mode, buffer, fd);
                if (mode[i] == 'o')
                    mode_o_command(channel, mode, buffer, fd);
                if ((mode[i] == 'k' && channel->get_key() != "") || mode[i] == 'l')
                    addmode += mode[i];
            }
            else if (channel_mode.find(mode[i]) == std::string::npos)
                addmode += mode[i];
        }
        channel->set_mode(channel_mode + addmode);
        std::string userAnswer = print_user(find_user(fd));
        if (!addmode.empty())
            userAnswer += "MODE " + channel->get_channelname() + " +" + addmode;
        if (userAnswer.find("MODE") != std::string::npos)
            this->sendtoeveryone(userAnswer, channel);
    }
}

void    Server::mode_channel_handler(std::string buffer, int fd)
{
    std::string tmp(buffer);
    int j = 0;
    size_t i;

    std::string targetmsg = "";

    if ((i = tmp.find_first_not_of(" \t\r\n", 5)) != std::string::npos)
        targetmsg = tmp.substr(i, ((j = tmp.find_first_of(" \t\r\n", i)) - i));
    if (targetmsg.empty())
    {
        this->_sendMessage(send_codes(461, this, find_user(fd), "MODE", ""), this->clientfd[fd].fd);
        return ;
    }
    std::string idOfChannel = "#&+";
    j = tmp.find_first_not_of(" \t\r\n", j);
    std::string mode = "";
    if (tmp.find_first_of(" \t\r\n", j) != std::string::npos)
        mode = tmp.substr(j, (tmp.find_first_of(" \t\r\n", j) - j));
    if (!targetmsg.empty() && idOfChannel.find(targetmsg[0]) != std::string::npos)
    {
        if (this->getChannels().find(targetmsg) == this->getChannels().end())
            this->_sendMessage(send_codes(403, this, find_user(fd), targetmsg, ""), this->clientfd[fd].fd);
       // else if (find_user(fd)->get_mode().find('r') != std::string::npos)
         //   this->_sendMessage(send_codes(484, this, find_user(fd), "", ""), this->clientfd[fd].fd);
        else if (this->getChannels().find(targetmsg)->second->getChanOps().find(this->clientfd[fd].fd) == this->getChannels().find(targetmsg)->second->getChanOps().end())
            this->_sendMessage(send_codes(482, this, find_user(fd), targetmsg, ""), this->clientfd[fd].fd);
        else
            channel_mode(this->getChannels().find(targetmsg)->second, mode, fd, buffer);
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

void Server::topic_command(std::string buffer, int fd)
{
    std::string buf(buffer);
    size_t i, j = 0;
    std::string channelName = "";
    if ((i = buf.find_first_not_of(" \t\r\n", 6)) != std::string::npos)
        channelName = buf.substr(i, ((j = buf.find_first_of(" \t\r\n", i)) - i));
    if (channelName.empty())
        this->_sendMessage(send_codes(461, this, find_user(fd), "TOPIC", ""), this->clientfd[fd].fd);
    else if (this->getChannels().find(channelName) == this->getChannels().end())
        this->_sendMessage(send_codes(403, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
    else if (find_user(fd)->get_channels().find(channelName) == find_user(fd)->get_channels().end())
        this->_sendMessage(send_codes(442, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
    else if ((this->getChannels().find(channelName)->second->get_mode().find("t") != std::string::npos) && (find_user(fd)->get_mode().find('i') != std::string::npos)) // mode i idk if its correct
        this->_sendMessage(send_codes(484, this, find_user(fd), "", ""), this->clientfd[fd].fd);
    else if ((this->getChannels().find(channelName)->second->get_mode().find("t") != std::string::npos) && (this->getChannels().find(channelName)->second->getChanOps().find(fd) == this->getChannels().find(channelName)->second->getChanOps().end()))
        this->_sendMessage(send_codes(482, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
    else
    {
        std::string topic = "";
        if ((j = buf.find_first_not_of(" \t\r\n", j)) != std::string::npos)
        {
            if (buf[j] == ':')
                j++;
            topic = buf.substr(j, (buf.find_first_of("\r\n", j) - j));
        }
        if (topic.empty())
            this->_sendMessage(send_codes(332, this, find_user(fd), channelName, this->getChannels().find(channelName)->second->get_topic()), this->clientfd[fd].fd);
        else
        {
            this->getChannels().find(channelName)->second->set_topic(topic);
            this->_sendMessage(send_codes(332, this, find_user(fd), channelName, this->getChannels().find(channelName)->second->get_topic()), this->clientfd[fd].fd);
            std::string userAnswer = print_user(find_user(fd));
            userAnswer += "TOPIC " + this->getChannels().find(channelName)->second->get_topic();
            sendtoeveryone(userAnswer, this->getChannels().find(channelName)->second);
        }
    }
}


void Server::kick_command(std::string buffer, int fd)
{
    std::string buf(buffer);
    int kickCount = std::count(buf.begin(), buf.end(), '\n');
    int k = 0;
    size_t i, j;
    std::string channelsName = "";
    if ((i = buf.find_first_not_of(" \t\r\n", 5)) != std::string::npos)
        channelsName = buf.substr(i, ((j = buf.find_first_of(" \t\r\n", i)) - i));
    if (channelsName.empty())
    {
        this->_sendMessage(send_codes(461, this, find_user(fd), "KICK", ""), this->clientfd[fd].fd);
        return;
    }
    int nbOfChannels = 1 + std::count(channelsName.begin(), channelsName.end(), ',');
    j = buf.find_first_not_of(" \t\r\n", j);
    k = j;
    for (int l = 0; l < kickCount; l++)
    {
        std::string usersNick = buf.substr(k, (buf.find_first_of(" \t\r\n", k) - k));
        int nbUsers = 1 + std::count(usersNick.begin(), usersNick.end(), ',');
        if (usersNick.empty())
        {
            this->_sendMessage(send_codes(461, this, find_user(fd), "KICK", ""), this->clientfd[fd].fd);
            return;
        }
        for (int j = 0; j < nbUsers; j++)
        {
            std::string userNick = usersNick.substr(0, usersNick.find(","));
            usersNick.erase(0, usersNick.find(",") + 1);
            for (int i = 0; i < nbOfChannels; i++)
            {
                std::string tmp = channelsName;
                std::string channelName = tmp.substr(0, tmp.find(","));
                tmp.erase(0, tmp.find(",") + 1);
                if (this->getChannels().find(channelName) == this->getChannels().end())
                {
                    if (j == 0)
                        this->_sendMessage(send_codes(403, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
                    continue;
                }
                if (this->getChannels().find(channelName)->second->searchuserbyname(find_user(fd)->get_nickname()) == -1)
                {
                    this->_sendMessage(send_codes(442, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
                    continue;
                }
                if (find_user(fd)->get_mode().find('r') != std::string::npos)
                {
                    this->_sendMessage(send_codes(484, this, find_user(i), "", ""), this->clientfd[fd].fd);
                    continue ;
                }
                if (!this->getChannels().find(channelName)->second->isChanop(this->clientfd[fd].fd))
                {
                    this->_sendMessage(send_codes(482, this, find_user(fd), channelName, ""), this->clientfd[fd].fd);
                    continue ;
                }
                int userToKickSd;
                if ((userToKickSd = this->getChannels().find(channelName)->second->searchuserbyname(userNick)) == -1)
                {
                    this->_sendMessage(send_codes(441, this, find_user(fd), userNick, channelName), this->clientfd[fd].fd);
                    continue;
                }
                std::string userAnswer = print_user(find_user_byfd(userToKickSd));
                userAnswer += "PART " + channelName;
                sendtoeveryone(userAnswer, this->getChannels().find(channelName)->second);
                this->getChannels().find(channelName)->second->rm_user(userToKickSd);
                if (this->getChannels().find(channelName)->second->getUsersNb() == 0)
                {
                    delete this->getChannels().find(channelName)->second;
                    this->getChannels().erase(channelName);
                }
                find_user_byfd(userToKickSd)->get_channels().erase(channelName);
            }
        }
        if (buf.find('\r') != std::string::npos)
            buf.erase(0, buf.find("\r\n") + 2);
        else
            buf.erase(0, buf.find('\n') + 1);
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
        if (this->getChannels().find(channelName)->second->get_mode().find("k") != std::string::npos)
        {
            if (key.empty() && this->getChannels().find(channelName)->second->get_key().compare("") != 0)
            {
                this->_sendMessage(send_codes(461, this, find_user(i), channelName, ""), this->clientfd[i].fd);
                return;
            }
            if (this->getChannels().find(channelName)->second->get_key().compare(key) != 0 && this->getChannels().find(channelName)->second->get_key().compare("") != 0)
            {
                this->_sendMessage(send_codes(475, this, find_user(i), channelName, ""), this->clientfd[i].fd);
                continue;
            }
        }
        if (this->getChannels().find(channelName)->second->get_mode().find("l") != std::string::npos)
        {
            if (this->getChannels().find(channelName)->second->get_maxUser() <= this->getChannels().find(channelName)->second->getUsersNb())
            {
                this->_sendMessage(send_codes(471, this, find_user(i), channelName, ""), this->clientfd[i].fd);
                continue;
            }
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
}

void    Server::mode_o_command(Channel *channel, std::string mode, std::string buffer, int fd)
{
    int i = 0;
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t')
        {
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            j++;
            i--;
        }
    }
    std::string name = buffer.substr(i, (buffer.find_first_of(" \t\r\n", i) - i));
    int userSd = channel->searchuserbyname(name);
    if (userSd == -1)
    {
        this->_sendMessage(send_codes(441, this, find_user(fd), name, channel->get_channelname()), this->clientfd[fd].fd);
        return;
    }
    channel->rm_user(userSd);
    if (mode[0] == '-')
        channel->addUser(userSd, find_user_byfd(userSd));
    else
        channel->addChanops(userSd, find_user_byfd(userSd));
    std::string userAnswer = print_user(find_user(fd));
    userAnswer += "MODE " + channel->get_channelname() + " " + mode + " " + name;
    this->_sendMessage(userAnswer, userSd);

}

void    Server::mode_l_command(Channel *channel, std::string mode, std::string buffer, int fd)
{
    std::cout << "in mode l before return " << std::endl;
    if (mode[0] == '-')
    {
	    channel->set_maxUser(-1);
	    return;
    }
    std::cout << "in mode l after return " << std::endl;
    int i = 0;
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t')
        {
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            j++;
            i--;
        }
    }
    std::cout << "in mode l after while " << std::endl;

    std::string name = buffer.substr(i, buffer.find('\r') != std::string::npos ? buffer.length() - 2 - i : buffer.length() - 1 - i);
    int maxUser = std::strtoul(name.c_str(), NULL, 0);
    if (maxUser < 0)
    {
	    this->_sendMessage("Max user key must be superior to 0.", this->clientfd[fd].fd);
	    return;
    }
    channel->set_maxUser(maxUser);
}

void    Server::mode_k_command(Channel *channel, std::string mode, std::string buffer, int fd)
{
    std::cout << "dans mode k" << std::endl;
    if (mode[0] == '-')
    {
        channel->set_key("");
        //channel->rm_mode("k");
        return ;
    }
    std::cout << "dans mode k apr return" << std::endl;
    int i = 0;
    for (int j = 0; buffer[i] && j < 3; i++)
    {
        if (buffer[i] == ' ' || buffer[i] == '\t')
        {
            while (buffer[i] == ' ' || buffer[i] == '\t')
                i++;
            j++;
            i--;
        }
    }
    std::string key = buffer.substr(i, (buffer.find_first_of(" \t\r\n", i) - i));
    if (key.compare("x") == 0)
    {
        std::cout << "dans mode k x" << std::endl;
        this->_sendMessage(send_codes(467, this, find_user(fd), channel->get_key(), ""), this->clientfd[fd].fd);
    }
    else
    {
        if (!key.empty())
            channel->set_key(key);
        else
        {
            std::cout << "wrong key dans mode k" << std::endl;
            std::string keycmp = print_user(find_user(fd));
            this->_sendMessage(keycmp + ": Wrong key!", this->clientfd[fd].fd);
        }
    }   
}

void Server::oper_command(std::string buffer, int fd)
{
    std::string buf(buffer);
    int j = 0;
    size_t i;
    std::string user;
    if ((i = buf.find_first_not_of(" \t\r\n", 5)) != std::string::npos)
        user = buf.substr(i, ((j = buf.find_first_of(" \t\r\n", i)) - i));
    if (user.empty())
    {
        this->_sendMessage(send_codes(461, this, find_user(fd), "OPER", ""), this->clientfd[fd].fd);
        return;
    }
    if (this->searchUserby_nickname(user) == -1)
    {
        this->_sendMessage(send_codes(401, this, find_user(fd), user, ""), this->clientfd[fd].fd);
        return;
    }
    j = buf.find_first_not_of(" \t\r\n", j);
    std::string password = buf.substr(j, (buf.find_first_of(" \t\r\n", j) - j));
    if (password.empty())
    {
        this->_sendMessage(send_codes(461, this, find_user(fd), "OPER", ""), this->clientfd[fd].fd);
        return;
    }
    if (password.compare(OPERPWD) == 0)
    {
        std:: string userAnswer = print_user(find_user_byfd(this->searchUserby_nickname(user)));
        userAnswer += "MODE " + find_user_byfd(this->searchUserby_nickname(user))->get_nickname() + " +o";
        find_user_byfd(this->searchUserby_nickname(user))->set_mode(find_user_byfd(this->searchUserby_nickname(user))->get_mode() + "o");
        this->_sendMessage(userAnswer, this->searchUserby_nickname(user));
        this->_sendMessage(send_codes(381, this, find_user_byfd(this->searchUserby_nickname(user)), "", ""), this->searchUserby_nickname(user));
    }
    else
        this->_sendMessage(send_codes(464, this, find_user_byfd(this->searchUserby_nickname(user)), "", ""), this->clientfd[fd].fd);
}

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