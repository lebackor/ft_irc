#include "bot.hpp"

void sendMessage(std::string message, int sd); 

Bot::Bot() : _name("FarksBOT")
{}

Bot::~Bot(){}


std::string Bot::getName(){
    return this->_name;
}

std::string print_userbot()
{
    return (":FarksBOT!Farks@BOT ");
}

void    Bot::connectBot(Server *serv, Channel *chan, int sd){

    if (chan->isChanop(sd)){
        if (chan->getBotStatus() == false){
            chan->setBotStatus();
            serv->sendtoeveryone(print_userbot() + " JOIN " + chan->get_channelname(), chan);
        }
        else{
            sendMessage(print_userbot() + " PRIVMSG " + chan->get_channelname() + " :Arrête de m'appeler stp je suis déja la", sd);
        }
    }
    else{
        sendMessage(send_codes(482, serv, serv->getUsers().find(sd)->second, chan->get_channelname(), ""), sd);
    }

}


void    Bot::quitBot(Server *serv, Channel *chan, int sd){
    if (chan->isChanop(sd)){
        if (chan->getBotStatus() == true){
            chan->setBotStatus();
            serv->sendtoeveryone(print_userbot() + " PART " + chan->get_channelname(), chan);
        }
        else{
            sendMessage(print_userbot() + " PRIVMSG " + chan->get_channelname() + " :Je me récupère laisse moi stp", sd);
        }
    }
    else
        sendMessage(send_codes(482, serv, serv->getUsers().find(sd)->second, chan->get_channelname(), ""), sd);
}

void sendMessage(std::string message, int sd) // same as server->_sendMessage i'm just too lazy to make this function public
{
	message += "\r\n";
	if (send(sd, message.c_str(), message.length(), 0) < 0)
		throw std::runtime_error("Error sending message.");
	std::cout << "Server sent this msg: " << message << std::endl;
}

void    Bot::commandBotHandler(Server *serv, Channel *chan, int sd, std::string buffer){
    if (buffer.find("bot") != std::string::npos && chan->getBotStatus() == false)
        this->connectBot(serv, chan, sd);
    else if (buffer.find("leave") != std::string::npos && chan->getBotStatus() == true)
        this->quitBot(serv, chan, sd);
    else if (chan->getBotStatus() == true && buffer.find("!bot") == std::string::npos)
    {

        if (buffer.find("test") != std::string::npos)
            serv->sendtoeveryone(print_userbot() + " PRIVMSG " + chan->get_channelname() + " Hello " + serv->find_user_byfd(sd)->get_nickname() + " !", chan);
    }
    else
        sendMessage(print_userbot() + " PRIVMSG " + chan->get_channelname() + " :Je me récupère laisse moi stp", sd);

}

void    Bot::sendgreetings(Server *serv, Channel *chan, int sd){

    if (chan->getBotStatus() == true)
    {
        serv->sendtoeveryone(print_userbot() + " PRIVMSG " + chan->get_channelname() + " Everybody say hello to our friend " + serv->find_user_byfd(sd)->get_nickname() + " !", chan);
    }
    else
        return ;
}
