#include "server.hpp"

Server::Server(){

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

void Server::setUsers(int fd, User *user)
{
	this->_users.insert(std::make_pair(fd, user));
}

void    Server::error(const char *msg)
{
    perror(msg);
    exit(1);
}

bool	Server::firstConnection(int i)
{
	(void) i;
//	if (_users[clientfd[i].fd]) //look in the map if my
//		return false;

	std::string buffer_str(_buffer);
	
    bool hasCapLS = buffer_str.find("CAP LS") != std::string::npos;
    bool hasNick = buffer_str.find("NICK ") != std::string::npos;
    bool hasUser = buffer_str.find("USER ") != std::string::npos;

	if (hasCapLS == true && hasNick == true && hasUser == true)
	{
		setUserInfo();
		return true;
	}

		// GERER TT LES CAS OU CAP LS ETC NE SENVOIENT PAS DUN COUP MAIS SPLIT EN 2 OU 3
	/*if (hasCapLS == true && hasNick == false && hasUser == false)
	if (hasCapLS == false && hasNick == true && hasUser == true)
	if (hasCapLS == false && hasNick == false && hasUser == true)
	if (hasCapLS == false && hasNick == true && hasUser == false)
	if (hasCapLS == false && hasNick == false && hasUser == true)
	if (hasCapLS == true && hasNick == true && hasUser == false)
*/

return false;
}

void	Server::setUserInfo()
{
    std::string nickname;
    std::string username;
    std::string hostname;
    std::string realname;

    std::istringstream iss(_buffer);
    std::string line;

    while (std::getline(iss, line)) {
        std::istringstream lineIss(line);
        std::string command;
        lineIss >> command;

        if (command == "NICK") {
            lineIss >> nickname;	
        } if (command == "USER") {
            std::string temp;
            lineIss >> username >> temp >> hostname;
            std::getline(lineIss, realname);

            if (!realname.empty() && realname[0] == ' ' && realname.length() > 1 && realname[1] == ':') {
                realname = realname.substr(2); // Ignore les deux premiers caractères
            }
        }
    }
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
		//je dois trouver un moyen de split buffer en 2 pour recup en index 0 la commande puis en index 1 le but(join, part, nick etc) if ()
		std::cout << "Client " << i << ": " << this->_buffer << std::endl;

		if (firstConnection(i) == true)
		{
			std::map<int, User*> tmp = getUsers();
			std::string user_name = tmp[this->clientfd[i].fd]->get_realname();
			if (tmp.empty())
				std::cout << "erreur ya pas de nom" << std::endl;
			else
				std::cout << "User: " << user_name << "created\n"; // Ajoutez cette ligne pour vérifier le contenu de user_name
		}
		else if (firstConnection(i) == false)// dire que c pas caps ni user ni nick
		{
			std::string tmp(_buffer);
			if (tmp.find("PING ") != std::string::npos)
				pong_command(tmp, i);
			else if (tmp.find("PART ")!= std::string::npos)
				part_command(tmp, i);
			else if(tmp.find("JOIN ") != std::string::npos)
				join_command(tmp, i);
			else if (tmp.find("JOIN ")== std::string::npos)
			{
				for (int j = 1; j <= MAX_CLIENTS; j++)
				{
					if ((i != j) && this->clientfd[j].fd != 0)
						this->_sendMessage(this->_buffer, this->clientfd[j].fd); // envoie le msg a tt les clients mais je verifie pas le channel etc a peaufiner
				}	
			}
			
		}
	}

}

