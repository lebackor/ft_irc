#include "server.hpp"


Server::Server(std::string port, std::string password) : _port(port), _password(password)
{

}

Server::~Server(){
	close(this->_serverSocket);
}

int        Server::get_newClientSocket(){
	return (this->_newClientSocket);
}
struct pollfd	Server::get_clientfd()
{
	return (*this->clientfd);
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

std::string Server::receiveMessage(int sd)
{
	std::string buf = "";
	memset(_buffer, 0, 1024);
	while ((buf += _buffer).find('\n') == std::string::npos)
		if (recv(sd, _buffer, 1024, 0) < 0)
			throw std::runtime_error("Error receiving message");
	std::cout << buf << std::endl;
	return buf;
}

void	Server::check_connection()
{
	std::string ret(_newbuff + _buffer);
	size_t occ;
	size_t firstOcc;
	bool isPassGood = false, isNickGood = false, isUserGood = false;
	std::string nick = "", user = "", host = "", serverName = "", realName = "", pass = "";

	//ret = this->receiveMessage(this->_newClientSocket);
	if (((ret.find("CAP LS") != std::string::npos && ret.find("PASS ") == std::string::npos) || (ret.find("CAP LS") != std::string::npos && ret.find("PASS ") == std::string::npos && ret.find("NICK ") != std::string::npos)) && ret.find("USER ") == std::string::npos)
	{
		std::cout << "there" << std::endl;
	//	ret = this->receiveMessage(this->_newClientSocket);
	}
	if ((occ = ret.find("PASS ")) != std::string::npos)
	{
		std::cout << "IN PASS" << std::endl;
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
				std::cout << "EMPTY  PASSWORD" << std::endl;
				this->_sendMessage(send_codes(461, this, NULL, "PASS", ""), this->_newClientSocket);
				//close(this->_newClientSocket);
			}
			else if (pass.compare(this->_password) != 0)
			{
				std::cout << "BAD PASSWORD" << std::endl;
				this->_sendMessage("WRONG PASSWORD", this->_newClientSocket);
				//close(this->_newClientSocket);
			}
			else
			{
				_newbuff += ret;
				std::cout << "GOOD PASSWORD" << std::endl;
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
		std::cout << "IN NICK" << std::endl;
	//	if (ret.find("NICK ") == std::string::npos)
			//ret = this->receiveMessage(this->_newClientSocket);
		if ((occ = ret.find("NICK ")) != std::string::npos)
		{
			if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == std::string::npos)
			{
				std::cout << "jsp dnickname" << std::endl;
				this->_sendMessage(send_codes(432, this, NULL, nick, ""), this->_newClientSocket);
			//	close(this->_newClientSocket);
			}
			else
			{
				nick = ret.substr(firstOcc, ret.find_first_of("\r\n", firstOcc) - firstOcc);
				nick = nick.substr(0, nick.find_last_not_of(" \t\r\n", nick.size()) + 1);
				if (!nicknameIsValid(nick))
				{
					std::cout << "nickname deja use" << std::endl;
					this->_sendMessage(send_codes(432, this, NULL, nick, ""), this->_newClientSocket);
				//	close(this->_newClientSocket);	
				}
				else if (nicknameAlreadyUse(nick))
				{
					std::cout << "nickname deja use" << std::endl;
					this->_sendMessage(send_codes(433, this, NULL, nick, ""), this->_newClientSocket);
					this->_sendMessage("Please try reconnect with an available nickname.", this->_newClientSocket);
				//	close(this->_newClientSocket);
				}
				else
				{
					_newbuff += ret;
					std::cout << "GOOD NICNAMAE" << std::endl;
					isNickGood = true;
				}
			}
		}
		else
		{
				std::cout << "pas dnickname" << std::endl;
			this->_sendMessage("You have to enter a nickname\nUsage: NICK [nickname]", this->_newClientSocket);
			//close(this->_newClientSocket);
		}
		if (isUserGood == false && isNickGood == true)
		{
			std::cout << "IN USER" << std::endl;
			if (ret.find("USER ") == std::string::npos)
			{
				std::cout << "PAS TROVUER USER" << std::endl;
			//	ret = this->receiveMessage(this->_newClientSocket);
			}
			if ((occ = ret.find("USER ")) != std::string::npos)
			{
				int i = 0;
				if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == std::string::npos)
				{
					std::cout << "error 211 " << std::endl;
					this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
				}
				else
				{
					user = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
					if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == std::string::npos)
					{
								std::cout << "error 217 " << std::endl;
						this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
					}
					else
					{
						host = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
						if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == std::string::npos)
						{
							std::cout << "error 223 " << std::endl;
							this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
						}
						
						else
						{
							serverName = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
							if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == std::string::npos)
							{
								std::cout << "error 229 " << std::endl;
								this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
							}
							else
							{
								std::cout << "parse un bon user " << std::endl;
								realName = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
								realName = realName.substr(0, realName.find_last_not_of(" \t\r\n", realName.size()) + 1);
							}
						}
					}
				}
				if (!(user.empty() || host.empty() || serverName.empty() || realName.empty()))
				{
					_newbuff += ret;
					std::cout << "USER GOOD " << std::endl;
					isUserGood = true;
				}
			}
		}
		if (isUserGood == false && isNickGood == true)
		{
			std::cout << "pas un bon user " << std::endl;
			this->_sendMessage("Usage: USER [username] [hostname] [serverName] [realName]", this->_newClientSocket);
			//close(this->_newClientSocket);
		}
	}
	if (isPassGood == true && _users.size() < 10 && isNickGood == true && isUserGood == true)
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
	else if (isPassGood == true && isNickGood == true && isUserGood == true)
		this->_sendMessage(send_codes(005, this, NULL, nick, ""), this->_newClientSocket);
}

bool	Server::firstConnection(int i)
{

	if (isThereanUser(this->clientfd[i].fd) == true) //gérer que a la disconnect on doit delete le fd et la class user de la map
		return false;	

	std::string buffer_str(_buffer);
	
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

void	Server::setUserInfo()
{
    std::string nickname;
    std::string username;
    std::string hostname;
    std::string realname;
	std::string servername;

    std::istringstream iss(_buffer);
    std::string line;

    while (std::getline(iss, line)) {
        std::istringstream lineIss(line);
        std::string command;
        lineIss >> command;

        if (command == "NICK") {
            lineIss >> nickname;	
        } if (command == "USER") {
            lineIss >> username >> servername >> hostname;
            std::getline(lineIss, realname);

            if (!realname.empty() && realname[0] == ' ' && realname.length() > 1 && realname[1] == ':') {
                realname = realname.substr(2); // Ignore les deux premiers caractères
            }
        }
    }
	this->setServername(servername);
	User *user = new User(nickname, username, hostname, realname);
	this->setUsers(this->get_newClientSocket(), user);
	if (_users[get_newClientSocket()])
	{
		this->_sendMessage(this->_welcolmeirssi(001), this->_newClientSocket);
		this->_sendMessage(this->_welcolmeirssi(002), this->_newClientSocket);
		this->_sendMessage(this->_welcolmeirssi(003), this->_newClientSocket);
		this->_sendMessage(this->_welcolmeirssi(004), this->_newClientSocket);
	}
	else
		std::cerr << "Error user class not defined, it should be" << std::endl;
}

std::string Server::_welcolmeirssi(int code)
{
	std::stringstream ss;
	ss << code;
	std::string codestr = ss.str();
    if (code < 10)
        codestr.insert(0, 2, '0');
    else if (code < 100)
	{
        codestr.insert(0, 1, '0');
	}

	std::map<int, User*> tmp = getUsers();

	std::string nickname = tmp[this->get_newClientSocket()]->get_nickname();
	std::string username = tmp[this->get_newClientSocket()]->get_username();
	std::string hostname = tmp[this->get_newClientSocket()]->get_hostname();
	std::string ret;
	ret += ":" + hostname + " " + codestr + " " + "*" + " ";	
	switch (code)
	{
        case 001:
            ret += RPL_WELCOME(nickname, username, hostname);			
			break;
		case 002:
            ret += RPL_YOURHOST(hostname);
			break;
        case 003:
            ret += RPL_CREATED;
			break;
        case 004:
            ret += RPL_MYINFO(hostname);
			break;		
	}
	return ret;
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
	int bytesRead = recv(this->clientfd[i].fd, this->_buffer, sizeof(this->_buffer), 0);
	
	if (bytesRead <= 0)
	{
		std::cout << "Client " << i << " has been disconnected" << std::endl;
		close(this->clientfd[i].fd);
		memset(&this->clientfd[i], 0, sizeof(this->clientfd[i]));
	}
	else
	{
		this->_buffer[bytesRead] = '\0';
		std::cout << "Client " << i << ": " << this->_buffer << std::endl;
		bool firstconnection = firstConnection(i);
		std::map<int, User*> tmp_user = getUsers();
		if (firstconnection == true)
		{
			check_connection();
		//	std::string user_name = tmp_user[clientfd[i].fd]->get_username();
		//	if (!tmp_user[clientfd[i].fd])
		//		std::cout << "erreur ya pas de nom" << std::endl;
		//	else
		//		std::cout << "User: " << user_name << "created\n"; // Ajoutez cette ligne pour vérifier le contenu de user_name
		}
		else if (firstconnection == false)// dire que c pas caps ni user ni nick
		{
			std::string tmp(_buffer);
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

}

