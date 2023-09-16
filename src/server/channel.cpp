#include "channel.hpp"

#include <sstream>

const char* const Channel::CHANMODES = "itlko";

Channel::Channel(std::string channelName, Client* user)
{
	_channelName = channelName;
	_channelOperators.push_back(user);
	_topic = "";
	_modes = 0;
}

bool Channel::addUser(Client* user, std::string key)
{
	if(isUserInChannel(user))
		return false;
	// if invite-only mode
	if(_modes & MODE_INVITE_ONLY)
	{
		for(std::vector<Client*>::iterator it = _invitedUserList.begin(); it != _invitedUserList.end(); ++it)
		{
			if(*it == user)
			{
				_userList.push_back(user);
				return true;
			}
		}
		return false;
	}
	// if key-required mode
	if(_modes & MODE_KEY_REQUIRED && _key != key)
	{
		return false;
	}
	// if limit-user mode
	if(_modes & MODE_LIMIT_USERS && _userList.size() >= _maxUsers) // _maxUsers = 0 shouln't happen when setting the mode
		return false;
	_userList.push_back(user);
	return true;
}

bool Channel::removeUser(Client* user)
{
	for(std::vector<Client*>::iterator it = _userList.begin(); it != _userList.end(); ++it)
	{
		if(*it == user)
		{
			_userList.erase(it);
			break;
		}
	}
	return true;
}

bool Channel::isUserInChannel(Client* user) const
{
	for(std::vector<Client*>::const_iterator it = _userList.begin(); it != _userList.end(); ++it)
	{
		if(*it == user)
			return true;
	}
	return false;
}

bool Channel::isUserChannelOperator(Client* user) const
{
	for(std::vector<Client*>::const_iterator it = _channelOperators.begin(); it != _channelOperators.end(); ++it)
	{
		if(*it == user)
			return true;
	}
	return false;
}

bool Channel::addChannelOperator(Client* op, Client* userTarget)
{
	if(!userTarget || (!op->isoper() && (!isUserInChannel(userTarget) || !isUserChannelOperator(op))))
		return false;
	_channelOperators.push_back(userTarget);
	return true;
}

bool Channel::removeChannelOperator(Client* op, Client* userTarget)
{
	if(!userTarget || (!isUserInChannel(userTarget) || !isUserChannelOperator(op) || !isUserChannelOperator(userTarget)))
		return false;
	for(std::vector<Client*>::iterator it = _channelOperators.begin(); it != _channelOperators.end(); ++it)
	{
		if(*it == userTarget)
		{
			_channelOperators.erase(it);
			break;
		}
	}
	return true;
}

bool Channel::setTopic(Client* user, std::string topic)
{
	if(_modes & MODE_TOPIC_PROTECTED || !isUserChannelOperator(user))
	{
		return false;
	}
	_topic = topic;
	return true;
}

bool Channel::sendChannelMessage(Client* user, std::string message)
{
	for(std::vector<Client*>::iterator it = _userList.begin(); it != _userList.end(); ++it)
	{
		if(*it != user)
			(*it)->send(":" + user->nick() + "!" + user->user() + "@" + user->host() + " PRIVMSG " + _channelName + " :" + message + "\r\n");
	}
	return true;
}

bool Channel::inviteUser(Client* op, Client* userTarget)
{
	if(!isUserChannelOperator(op) || isUserInChannel(userTarget))
		return false;
	if(_modes & MODE_INVITE_ONLY)
		_invitedUserList.push_back(userTarget);
	userTarget->send(":" + op->nick() + "!" + op->user() + "@" + op->host() + " INVITE " + userTarget->nick() + " " + _channelName + "\r\n");
	return true;
}

bool Channel::kickUser(Client* op, Client* userTarget, std::string reason)
{
	if(!isUserChannelOperator(op) || !isUserInChannel(userTarget))
		return false;
	removeUser(userTarget);
	if(reason == "")
		userTarget->send(":" + op->nick() + "!" + op->user() + "@" + op->host() + " KICK " + _channelName + " " + userTarget->nick() + "\r\n");
	else
		userTarget->send(":" + op->nick() + "!" + op->user() + "@" + op->host() + " KICK " + _channelName + " " + userTarget->nick() + " :" + reason + "\r\n");
	return true;
}

bool Channel::setUserLimit(const std::string& limit)
{
	if(_modes & MODE_LIMIT_USERS)
		return false;
	std::stringstream ss(limit);
	ss >> _maxUsers;
	if(ss.fail() || !ss.eof())
	{
		_maxUsers = 0;
		return false;
	}
	_modes |= MODE_LIMIT_USERS;
	return true;
}
