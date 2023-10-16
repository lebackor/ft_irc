/// paste









///



void Server::newConnection()
{

	//inform user of socket number - used in send and receive commands
	std::cout << "New connection , socket fd is " << this->_newClientSocket << " , ip is : " << inet_ntoa(_cliAddr.sin_addr) << " , port : " <<  ntohs(_cliAddr.sin_port) << std::endl;
	//send new connection greeting message
	std::string ret(_buffer);
	size_t occ;
	size_t firstOcc;
	bool isPassGood = false, isNickGood = false, isUserGood = false;
	std::string nick = "", user = "", host = "", serverName = "", realName = "", pass = "";

	//ret = this->receiveMessage(this->_newClientSocket);
//	if (((ret.find("CAP LS") != std::string::npos && ret.find("PASS ") == std::string::npos) || (ret.find("CAP LS") != std::string::npos && ret.find("PASS ") == std::string::npos && ret.find("NICK ") != std::string::npos)) && ret.find("USER ") == std::string::npos)
//		ret = this->receiveMessage(this->_newClientSocket);
	if ((occ = ret.find("PASS ")) != std::string::npos)
	{
		if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == std::string::npos)
		{
			this->_sendMessage(send_codes(461, this, NULL, "PASS", ""), this->_newClientSocket);
			close(this->_newClientSocket);
		}
		else
		{
			std::string sep = " \t\r\n";
			for (int i = 0; ret[firstOcc + i] && sep.find(ret[firstOcc + i]) == std::string::npos; i++)
				pass += ret[firstOcc + i];
			if (pass.empty())
			{
				this->_sendMessage(send_codes(461, this, NULL, "PASS", ""), this->_newClientSocket);
				close(this->_newClientSocket);
			}
			else if (pass.compare(this->_password) != 0)
			{
				this->_sendMessage("WRONG PASSWORD", this->_newClientSocket);
				close(this->_newClientSocket);
			}
			else
				isPassGood = true;
		}
	}
	else
	{
		this->_sendMessage("You need to enter a pass!", this->_newClientSocket);
		close(this->_newClientSocket);
	}
	if (isPassGood == true)
	{
		if (ret.find("NICK ") == std::string::npos)
			ret = this->receiveMessage(this->_newClientSocket);
		if ((occ = ret.find("NICK ")) != std::string::npos)
		{
			if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == std::string::npos)
			{
				this->_sendMessage(send_codes(432, this, NULL, nick, ""), this->_newClientSocket);
				close(this->_newClientSocket);
			}
			else
			{
				nick = ret.substr(firstOcc, ret.find_first_of("\r\n", firstOcc) - firstOcc);
				nick = nick.substr(0, nick.find_last_not_of(" \t\r\n", nick.size()) + 1);
				if (!nicknameIsValid(nick))
				{
					this->_sendMessage(send_codes(432, this, NULL, nick, ""), this->_newClientSocket);
					close(this->_newClientSocket);	
				}
				else if (nicknameAlreadyUse(nick))
				{
					this->_sendMessage(send_codes(433, this, NULL, nick, ""), this->_newClientSocket);
					this->_sendMessage("Please try reconnect with an available nickname.", this->_newClientSocket);
					close(this->_newClientSocket);
				}
				else
					isNickGood = true;
			}
		}
		else
		{
			this->_sendMessage("You have to enter a nickname\nUsage: NICK [nickname]", this->_newClientSocket);
			close(this->_newClientSocket);
		}
		if (isUserGood == false && isNickGood == true)
		{
			if (ret.find("USER ") == std::string::npos)
				ret = this->receiveMessage(this->_newClientSocket);
			if ((occ = ret.find("USER ")) != std::string::npos)
			{
				int i = 0;
				//username
				if ((firstOcc = ret.find_first_not_of(" \t\r\n", occ + 5)) == std::string::npos)
					this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
				else
				{
					user = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
					//hostname
					if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == std::string::npos)
						this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
					else
					{
						host = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
						//serverName
						if ((firstOcc = ret.find_first_not_of(" \t\r\n", i)) == std::string::npos)
							this->_sendMessage(send_codes(461, this, NULL, "USER", ""), this->_newClientSocket);
						else
						{
							serverName = ret.substr(firstOcc, (i = ret.find_first_of(" \t\r\n", firstOcc)) - firstOcc);
							//realName
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
					isUserGood = true;
			}
		}
		if (isUserGood == false && isNickGood == true)
		{
			this->_sendMessage("Usage: USER [username] [hostname] [serverName] [realName]", this->_newClientSocket);
			close(this->_newClientSocket);
		}
	}
	if (isPassGood == true && _users.size() < 10 && isNickGood == true && isUserGood == true && isAlive == true)
	{
		this->_serverName = serverName;
		User *newUser = new User(nick, user, host, realName);
		this->setUsers(this->_newClientSocket, newUser);
		std::cout << "Number of user connected on the server: " << this->_users.size() << std::endl;
		this->_sendMessage(send_codes(001, this, newUser, "", ""), this->_newClientSocket);
		this->_sendMessage(send_codes(002, this, newUser, "", ""), this->_newClientSocket);
		this->_sendMessage(send_codes(003, this, newUser, "", ""), this->_newClientSocket);
		this->_sendMessage(send_codes(004, this, newUser, "", ""), this->_newClientSocket);
		//add new socket to array of sockets
		
	//	int clientFlags = fcntl(this->_newClientSocket, F_GETFL, 0);
	//	fcntl(this->_newClientSocket, F_SETFL, clientFlags | O_NONBLOCK);
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			//if position is empty
			if (this->clientfd[i].fd == 0)
			{
				this->clientfd[i].fd = this->_newClientSocket;
				this->clientfd[i].events = POLLIN;
				break;
			}
		}
	}
	else if (isPassGood == true && isNickGood == true && isAlive == true && isUserGood == true)
		this->_sendMessage(send_codes(005, this, NULL, nick, ""), this->_newClientSocket);
}