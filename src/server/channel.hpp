#pragma once

#include "client.hpp"

#include <iostream>
#include <vector>

class Channel
{
public:
	Channel(std::string channelName, Client* user);
	~Channel();

	enum
	{
		MODE_INVITE_ONLY = 1 << 0,
		MODE_TOPIC_PROTECTED = 1 << 1,
		MODE_LIMIT_USERS = 1 << 2
	};

	const std::string& getChannelName() const { return _channelName; }
	const std::vector<Client*>& getUserList() const { return _userList; }
	const std::vector<Client*>& getChannelOperators() const { return _channelOperators; }
	const std::string& getCurrentTopic() const { return _topic; } // TOPIC #channelName
	
	bool isUserInChannel(Client* user) const;
	bool addUser(Client* user); // JOIN
	bool removeUser(Client* user); // PART

	bool isUserChannelOperator(Client* user) const;
	bool addChannelOperator(Client* op, Client* userTarget); // MODE +o
	bool removeChannelOperator(Client* op, Client* userTarget); // MODE -o

	bool setTopic(Client* op, std::string topic); // TOPIC #channelName :topic
	bool sendChannelMessage(Client* user, std::string message); // PRIVMSG #channelName :message

	bool inviteUser(Client* op, Client* userTarget); // INVITE user #channelName
	bool kickUser(Client* op, Client* userTarget, std::string reason = ""); // KICK #channelName user :reason

private:

	std::string _channelName;
	std::vector<Client*> _userList;
	std::vector<Client*> _channelOperators; // _channelOperators[0] is the channel creator
	std::string _topic;
	std::vector<Client*> _invitedUserList;

	int _modes; // bit field

	size_t _maxUsers;
	std::string _topic;
	std::string _key;
};
