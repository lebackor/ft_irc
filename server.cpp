#include "server.hpp"


Server::Server(std::string port, std::string password) : _port(port), _password(password)
{

}

Server::~Server(){

	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		//if (it->second)
		//	it->second->~Channel();
	    delete it->second;
	}
	if (!_channels.empty())
		_channels.clear();

	for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		//if (it->second)
		//	it->second->~User();
	    delete it->second;
	}
	if (!_users.empty())
		_users.clear();

	if (!_buffer_sd.empty())
		_buffer_sd.clear();
	
close(this->_serverSocket);
}

int        Server::get_newClientSocket(){
	return (this->_newClientSocket);
}
struct pollfd	Server::get_clientfd()
{
	return (*this->clientfd);
}

std::map<int, std::string> & Server::getBufferSd(){
	return this->_buffer_sd;
}
void        Server::setBufferSd(int sd, std::string buffer){
	this->_buffer_sd.insert(std::make_pair(sd, buffer));
}

std::map<int, User*> & Server::getUsers(){
	return this->_users;
}

std::map<std::string, Channel*> & Server::getChannels(){
	return this->_channels;
}

void	Server::setChannels(std::string name, Channel *channel)
{
	this->_channels.insert(std::make_pair(name, channel));
}

void        Server::set_bufferstr(std::string str){
	this->_strbuffer = str;
}

std::string Server::get_bufferstr(){
	return this->_strbuffer;
}

void	Server::setServername(std::string servername)
{
	this->_serverName = servername;
}

std::string	Server::getServername()
{
	return this->_serverName;
}

bool    Server::isThereanUser(int fd)
{
	if (this->_users.find(fd) == this->_users.end())
		return (false);
	else
		return (true); 
}
std::string Server::getPort()
{
	return this->_port;
}

void Server::setUsers(int fd, User *user)
{
	this->_users.insert(std::make_pair(fd, user));
}

void    Server::error(const char *msg)
{
    perror(msg);
    exit(1);
}

User*	Server::find_user(int fd)
{
	return (this->getUsers()[this->clientfd[fd].fd]);
}

User*	Server::find_user_byfd(int fd)
{
	return (this->getUsers()[fd]);
}

bool Server::nicknameAlreadyUse(std::string nick)
{
    for (std::map<int, User *>::iterator it = this->getUsers().begin(); it != this->getUsers().end(); it++)
    {
        if (nick.compare(it->second->get_nickname()) == 0)
            return (true);
    }
    return (false);
}

void	Server::check_connection()
{
	std::string ret(_newbuff + _strbuffer);
	size_t occ;
	size_t firstOcc;
	bool isPassGood = false, isNickGood = false, isUserGood = false;
	std::string nick = "", user = "", host = "", serverName = "", realName = "", pass = "";


	if ((occ = ret.find("PASS ")) != std::string::npos)
	{
		if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == std::string::npos)
		{
			this->_sendMessage(send_codes(461, this, NULL, "PASS", ""), this->_newClientSocket);
	//		close(this->_newClientSocket);
		}
		else
		{
			std::string sep = " \t\r\n";
			for (int i = 0; ret[firstOcc + i] && sep.find(ret[firstOcc + i]) == std::string::npos; i++)
				pass += ret[firstOcc + i];
			if (pass.empty())
			{
				this->_sendMessage(send_codes(461, this, NULL, "PASS", ""), this->_newClientSocket);
				//close(this->_newClientSocket);
			}
			else if (pass.compare(this->_password) != 0)
			{
				this->_sendMessage("WRONG PASSWORD", this->_newClientSocket);
				//close(this->_newClientSocket);
			}
			else
			{
				_newbuff += ret;
				isPassGood = true;
			}
		}
	}
	else
	{
		this->_sendMessage("You need to enter a pass!", this->_newClientSocket);
		//close(this->_newClientSocket);
	}
	if (isPassGood == true)
	{
		if ((occ = ret.find("NICK ")) != std::string::npos)
		{
			if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == std::string::npos)
			{
				this->_sendMessage(send_codes(432, this, NULL, nick, ""), this->_newClientSocket);
			//	close(this->_newClientSocket);
			}
			else
			{
				nick = ret.substr(firstOcc, ret.find_first_of("\r\n", firstOcc) - firstOcc);
				nick = nick.substr(0, nick.find_last_not_of(" \t\r\n", nick.size()) + 1);
				if (!nicknameIsValid(nick))
				{
					this->_sendMessage(send_codes(432, this, NULL, nick, ""), this->_newClientSocket);
				//	close(this->_newClientSocket);	
				}
				else if (nicknameAlreadyUse(nick))
				{
					this->_sendMessage(send_codes(433, this, NULL, nick, ""), this->_newClientSocket);
					this->_sendMessage("Please try reconnect with an available nickname.", this->_newClientSocket);
				//	close(this->_newClientSocket);
				}
				else
				{
					_newbuff += ret;
					isNickGood = true;
				}
			}
		}
		else
		{
			this->_sendMessage("You have to enter a nickname\nUsage: NICK [nickname]", this->_newClientSocket);
			//close(this->_newClientSocket);
		}
		if (isUserGood == false && isNickGood == true)
		{
			if ((occ = ret.find("USER ")) != std::string::npos)
			{
				int i = 0;
				if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == std::string::npos)
					this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
				else
				{
					user = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
					if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == std::string::npos)
						this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
					else
					{
						host = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
						if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == std::string::npos)
							this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
						else
						{
							serverName = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
							if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == std::string::npos)
								this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
							else
							{
								realName = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
								realName = realName.substr(0, realName.find_last_not_of(" \t\r\n", realName.size()) + 1);
							}
						}
					}
				}
				if (!(user.empty() || host.empty() || serverName.empty() || realName.empty()))
				{
					_newbuff += ret;
					isUserGood = true;
				}
			}
		}
		if (isUserGood == false && isNickGood == true)
		{
			this->_sendMessage("Usage: USER [username] [hostname] [serverName] [realName]", this->_newClientSocket);
			//close(this->_newClientSocket);
		}
	}
	if (isPassGood == true && _users.size() < 10 && isNickGood == true && isUserGood == true && running_status() == true)
	{
		_newbuff.erase(_newbuff.begin(), _newbuff.end());
		this->_serverName = serverName;
		User *newUser = new User(nick, user, host, realName);
		this->setUsers(this->_newClientSocket, newUser);
		std::cout << "Number of user connected on the server: " << this->_users.size() << std::endl;
		this->_sendMessage(send_codes(001, this, newUser, "", ""), this->_newClientSocket);
		this->_sendMessage(send_codes(002, this, newUser, "", ""), this->_newClientSocket);
		this->_sendMessage(send_codes(003, this, newUser, "", ""), this->_newClientSocket);
		this->_sendMessage(send_codes(004, this, newUser, "", ""), this->_newClientSocket);

	}
	else if (isPassGood == true && isNickGood == true && isUserGood == true && running_status() == true)
		this->_sendMessage(send_codes(005, this, NULL, nick, ""), this->_newClientSocket);
}

bool	Server::firstConnection(int i)
{

	if (isThereanUser(this->clientfd[i].fd) == true)
		return false;	

	std::string buffer_str(_strbuffer);
	
    bool hasCapLS = buffer_str.find("CAP LS") != std::string::npos;
	bool hasPASS = buffer_str.find("PASS ") != std::string::npos;
    bool hasNick = buffer_str.find("NICK ") != std::string::npos;
    bool hasUser = buffer_str.find("USER ") != std::string::npos;

	if (hasCapLS == true || hasNick == true || hasUser == true || hasPASS == true)
		return true;
	return false;
}

int Server::searchUserby_nickname(std::string nickname)
{
    for (std::map<int, User*>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
    {
        if (nickname.compare(it->second->get_nickname()) == 0)
            return it->first;
    }
    return (-1);
}

void Server::_sendMessage(std::string message, int sd)
{
	message += "\r\n";
	if (send(sd, message.c_str(), message.length(), 0) < 0)
		throw std::runtime_error("Error sending message.");
	std::cout << "Server has sent this message: " << message << std::endl;
}


void	Server::setserversocket()
{
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_serverSocket < 0)
    {
	    error("Error opening socket");
	}

	
	this->_portnb = atoi(this->av[1]);

    memset(&this->_servAddr, 0, sizeof(this->_servAddr));
	this->_servAddr.sin_family = AF_INET;
    this->_servAddr.sin_addr.s_addr = INADDR_ANY;
    this->_servAddr.sin_port = htons(this->_portnb);

	this->_cliLen = sizeof(this->_cliAddr);
    if (bind(this->_serverSocket, (struct sockaddr *) &this->_servAddr, sizeof(this->_servAddr)) < 0)
	{
    	error("Binding failed.");
	}

	listen(this->_serverSocket, 5); // 5 is the number max of clients can connect simultanately


	
	memset(this->clientfd, 0, sizeof(this->clientfd));

	this->clientfd[0].fd = this->_serverSocket;
	this->clientfd[0].events = POLLIN;

	int flags = fcntl(this->_serverSocket, F_GETFL);
	fcntl(this->_serverSocket, F_SETFL, flags | O_NONBLOCK);

}

void Server::user_disconnect(int sd)
{
    std::set<std::string> userChannels = this->getUsers().find(sd)->second->get_channels();
    // erase user from each channel;
    for (std::set<std::string>::iterator it = userChannels.begin(); it != userChannels.end(); it++)
    {
        std::string userAnswer = print_user(this->getUsers().find(sd)->second);
        userAnswer += "PART " + *it;
        sendtoeveryone(userAnswer, this->getChannels().find(*it)->second);
        this->getChannels().find(*it)->second->rm_user(sd);
        if (this->getChannels().find(*it)->second->getUsersNb() == 0)
        {
            delete this->getChannels().find(*it)->second;
            this->getChannels().erase(*it);
        }
    }
    this->getUsers().find(sd)->second->get_channels().clear();
    delete this->getUsers().find(sd)->second;
    this->getUsers().erase(sd);
	close(sd);
}


void	Server::acceptClientconnexion()
{

	this->_newClientSocket = accept(this->_serverSocket, (struct sockaddr*)&this->_cliAddr, &this->_cliLen);	
		if (this->_newClientSocket == -1)
			perror("Error accepting a connection");
		else
		{
			std::cout << "Server has detected a connection !" << std::endl;
			int clientFlags = fcntl(this->_newClientSocket, F_GETFL, 0);
			fcntl(this->_newClientSocket, F_SETFL, clientFlags | O_NONBLOCK);

			this->setBufferSd(this->_newClientSocket, "");

			for (int i = 1; i <= MAX_CLIENTS; i++)
			{
				if (this->clientfd[i].fd == 0)
				{
					this->clientfd[i].fd = this->_newClientSocket;
					this->clientfd[i].events = POLLIN;


					break;
				}
			}

		}
}


void	Server::recvClientMsg(int i)
{

	bool firstconnection = firstConnection(i);

	if (firstconnection == true && running_status() == true)
		check_connection();
	else if (firstconnection == false && running_status() == true)
	{
		std::string tmp(_strbuffer);
		if (tmp.find("PING ") != std::string::npos)
			pong_command(tmp, i);
		else if (tmp.find("PART ")!= std::string::npos)
			part_command(tmp, i);
		else if(tmp.find("JOIN ") != std::string::npos)
			join_command(tmp, i);
		else if (tmp.find("PRIVMSG ") != std::string::npos)
			priv_msg(tmp, i);
		else if (tmp.find("NICK ") != std::string::npos)
			nick_command(tmp, i);
		else if (tmp.find("OPER ") != std::string::npos)
			oper_command(tmp, i);
		else if (tmp.find("TOPIC ") != std::string::npos)
			topic_command(tmp, i);
		else if (tmp.find("KICK ") != std::string::npos)
			kick_command(tmp, i);
		else if(tmp.find("MODE ") != std::string::npos)
			mode_channel_handler(tmp, i);
		else if(tmp.find("INVITE ") != std::string::npos)
			invite_command(tmp, i);
	}

}

