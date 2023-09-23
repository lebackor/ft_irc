#include "channel.hpp"


Channel::Channel(std::string name) :_name(name){}

Channel::~Channel(){}

std::string Channel::get_channelname()
{
    return (_name);
}