#include "client.hpp"

Client::Client()
{
}

Client::~Client()
{
}

bool Client::receive(void* data, uint32 size)
{
	ssize_t r = _socket.receive(data, size);
	if(r == 0)
		return false;
	else if(r == -1)
		perror("recv");
	reinterpret_cast<byte*>(data)[r] = '\0';
	return true;
}
