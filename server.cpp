#include "server.hpp"

Server::Server(){

}

Server::~Server(){
	close(this->_serverSocket);
}


struct pollfd	Server::get_clientfd()
{
	return (*this->clientfd);
}

void    Server::error(const char *msg)
{
    perror(msg);
    exit(1);
}

bool	Server::firstConnection()
{
   std::istringstream iss(this->_buffer);
    std::vector<std::string> identificationLines;

    std::string nickname;
    std::string username;
    std::string hostname;
    std::string realname;
 //   std::string tmp;

    std::string line;
    while (std::getline(iss, line)) {
        identificationLines.push_back(line);
    }

    // Vérification de la commande CAP LS
    bool hasCapLS = false;
  

    for (std::vector<std::string>::const_iterator it = identificationLines.begin(); it != identificationLines.end(); ++it) {
        const std::string& identificationLine = *it;

        if (identificationLine.find("CAP LS") != std::string::npos) {
            hasCapLS = true;
        } else if (identificationLine.find("NICK ") != std::string::npos) {
            size_t pos = identificationLine.find("NICK ");
            if (pos != std::string::npos) {
                nickname = identificationLine.substr(pos + 5); // +5 pour sauter "NICK "
            }
        } else if (identificationLine.find("USER ") != std::string::npos) {
            size_t pos = identificationLine.find("USER ");
            if (pos != std::string::npos) {
                std::string userPart = identificationLine.substr(pos + 5); // +5 pour sauter "USER "
                std::istringstream userIss(userPart);
                userIss >> username >> username >> hostname;
                std::getline(userIss, realname);
                if (!realname.empty() && realname[0] == ':') {
					realname = realname.substr(1);
                }
            }
        }
    }
		std::cout << "Nickname: " << nickname << std::endl;
		std::cout << "Username: " << username << std::endl;
		std::cout << "Hostname: " << hostname << std::endl;
		std::cout << "Realname: " << realname << std::endl;
    // Vérifier si les informations d'identification de irssi sont présentes et la commande CAP LS
    if (hasCapLS && !nickname.empty() && !username.empty() && !hostname.empty() && !realname.empty()) {
        // Si les informations d'identification sont présentes, envoyez un message à irssi
        return true;
    } else {
        // Les informations d'identification de irssi ou la commande CAP LS ne sont pas présentes
		return false;
	}

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

	//std::string nickname, username, hostname, realname;
    std::string serv_name = SERVER_NAME;

   /* std::istringstream iss(this->_buffer);
    

    std::string line;
    while (std::getline(iss, line)) {
        std::istringstream lineIss(line);
        std::string command;
        lineIss >> command;
        
        if (command == "NICK") {
            lineIss >> nickname;
        } else if (command == "USER") {
            lineIss >> username >> username >> hostname;
            std::getline(lineIss, realname);
            if (!realname.empty() && realname[0] == ':') {
                realname = realname.substr(1);
            }
        }
    }
*/
    // Afficher les valeurs

    std::string nickname;
    std::string username;
    std::string hostname;
    std::string realname;

	std::string ret;
	ret += ":" + serv_name + " " + codestr + " " + "*" + " ";	
	nickname = "fuerza";
	username = "len";
	hostname = "len";


	switch (code)
	{
        case 001:
            ret += RPL_WELCOME(nickname, username, hostname);
			break;
		case 002:
            ret += RPL_YOURHOST;
			break;
        case 003:
            ret += RPL_CREATED;
			break;
        case 004:
            ret += RPL_MYINFO;
			break;		
	}
	return ret;
}

void Server::_sendMessage(std::string message, int sd)
{
	message += "\r\n";
	if (send(sd, message.c_str(), message.length(), 0) < 0)
		throw std::runtime_error("Error sending message.");
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

	std::cout << "Socket server initialized" << std::endl;
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

		if (firstConnection() == true)
		{
       		this->_sendMessage(this->_welcolmeirssi(001), this->_newClientSocket);
			this->_sendMessage(this->_welcolmeirssi(002), this->_newClientSocket);
			this->_sendMessage(this->_welcolmeirssi(003), this->_newClientSocket);
			this->_sendMessage(this->_welcolmeirssi(004), this->_newClientSocket);
		}
		else
		{
			// met le transfer pr les autres clients
			for (int j = 1; j <= MAX_CLIENTS; j++)
			{
				if (j != i && this->clientfd[j].fd != 0)
					this->_sendMessage(this->_buffer, this->clientfd[j].fd);
			}
		}

	}

// faire des conditins et des containers pr stocker les user et les channel et guetter si il est dans un channel pr envoyer vrmnt d msg et non renvoyer cap ls etc de lautre client qui vient de se log

}

