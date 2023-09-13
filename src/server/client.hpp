#pragma once

#include "net/socket.hpp"

#include <string>
#include <ctime>

class Client
{
public:
	Client();
	~Client();

	enum
	{
		MODE_I = 1,
		MODE_O = 2,
		MODE_S = 4,
		MODE_V = 8,
		MODE_REGISTERED = 16,
		MODE_PASSWORDED = 32
	};

	operator const int&() const { return _socket; }

	bool accept(const net::Socket& s);
	bool receive();
	bool send(const char* string) const;
	bool send(const char* data, uint32 size) const;
	bool send(const std::string& data) const;
	bool ping();

	const std::string& data() const { return _data; }

	bool password(const std::string& pass);
	bool setnick(const std::string& nick);
	bool setuser(const std::string& user, const std::string& host, const std::string& realname);
	bool oper(const std::string& user, const std::string& pass);
	void addmode(int mode) { _modes |= mode; }
	void delmode(int mode) { _modes &= ~mode; }
	int modes() const { return _modes; }

	const std::string& nick() const { return _nick; }
	const std::string& user() const { return _user; }
	const std::string& host() const { return _host; }
	const std::string& realname() const { return _realname; }

	bool passworded() const { return _modes & MODE_PASSWORDED; }
	bool registered() const { return _modes & MODE_REGISTERED; }

	std::time_t lastping() const { return _lastping; }
	std::time_t lastpinged() const { return _lastpinged; }

	bool flagDisconnect;

private:

	Client(const Client& obj);
	Client& operator=(const Client& obj);

	std::time_t _lastping;
	std::time_t _lastpinged;

	std::string _nick;
	std::string _user;
	std::string _host;
	std::string _realname;

	std::string _data;

	int _modes; // bit field

	net::Socket _socket;
};
