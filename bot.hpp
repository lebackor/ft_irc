#pragma once

#include "server.hpp"

class Server;
class Channel;


class Bot{
    public:

        std::string getName();

        void    connectBot(Server *serv, Channel *chan, int sd);
        void    quitBot(Server *serv, Channel *chan, int sd);
        void    commandBotHandler(Server *serv, Channel *chan, int sd, std::string buffer);
        void    sendgreetings(Server *serv, Channel *chan, int sd);
        void    sendbye(Server *serv, Channel *chan, int sd);
        Bot();
        ~Bot();

    private:
        std::string _name;

};