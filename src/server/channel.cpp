#include "channel.hpp"

Channel::Channel(std::string channelName, Client* user)
{
    _channelName = channelName;
    _userList.push_back(user);
    _channelOperators.push_back(user);
    _topic = "";
    //_inviteOnly = false; ???????????????
}

Channel::~Channel()
{
}

bool Channel::addUser(Client* user)
{
    if (isUserInChannel(user))
        return false;
    // if (_modes & MODE_INVITE_ONLY) ???????????????
    _userList.push_back(user);
    return true;
}

bool Channel::removeUser(Client* user)
{
    for (std::vector<Client*>::iterator it = _userList.begin(); it != _userList.end(); ++it)
    {
        if (*it == user)
        {
            _userList.erase(it);
            break;
        }
    }
}

bool Channel::isUserInChannel(Client* user) const
{
    for (std::vector<Client*>::const_iterator it = _userList.begin(); it != _userList.end(); ++it)
    {
        if (*it == user)
            return true;
    }
    return false;
}

bool Channel::isUserChannelOperator(Client* user) const
{
    for (std::vector<Client*>::const_iterator it = _channelOperators.begin(); it != _channelOperators.end(); ++it)
    {
        if (*it == user)
            return true;
    }
    return false;
}

bool Channel::addChannelOperator(Client* op, Client* userTarget)
{
    if (!isUserInChannel(userTarget) || !isUserChannelOperator(op))
        return false;
    _channelOperators.push_back(userTarget);
    return true;
}

bool Channel::removeChannelOperator(Client* op, Client* userTarget)
{
    if (!isUserInChannel(userTarget) || !isUserChannelOperator(op) || !isUserChannelOperator(userTarget))
        return false;
    for (std::vector<Client*>::iterator it = _channelOperators.begin(); it != _channelOperators.end(); ++it)
    {
        if (*it == userTarget)
        {
            _channelOperators.erase(it);
            break;
        }
    }
    return true;
}

bool Channel::setTopic(Client* op, std::string topic)
{
    if (!isUserChannelOperator(op))
        return false;
    _topic = topic;
    return true;
}

bool Channel::sendChannelMessage(Client* user, std::string message)
{
    if (!isUserInChannel(user))
        return false;
    for (std::vector<Client*>::iterator it = _userList.begin(); it != _userList.end(); ++it)
    {
        if (*it != user)
            (*it)->send(":" + user->nick() + "!" + user->user() + "@" + user->host() + " PRIVMSG " + _channelName + " :" + message + "\r\n");
    }
    return true;
}

bool Channel::inviteUser(Client* op, Client* userTarget)
{
    if (!isUserChannelOperator(op) || !isUserInChannel(userTarget))
        return false;
    // if (_modes & MODE_INVITE_ONLY) ???????????????
    //     _invitedUserList.push_back(userTarget);
    userTarget->send(":" + op->nick() + "!" + op->user() + "@" + op->host() + " INVITE " + userTarget->nick() + " " + _channelName + "\r\n");
    return true;
}

bool Channel::kickUser(Client* op, Client* userTarget, std::string reason = "")
{
    if (!isUserChannelOperator(op) || !isUserInChannel(userTarget))
        return false;
    for (std::vector<Client*>::iterator it = _userList.begin(); it != _userList.end(); ++it)
    {
        if (*it == userTarget)
        {
            _userList.erase(it);
            break;
        }
    }
    if (reason == "")
        userTarget->send(":" + op->nick() + "!" + op->user() + "@" + op->host() + " KICK " + _channelName + " " + userTarget->nick() + "\r\n");
    else
        userTarget->send(":" + op->nick() + "!" + op->user() + "@" + op->host() + " KICK " + _channelName + " " + userTarget->nick() + " :" + reason + "\r\n");
    return true;
}
