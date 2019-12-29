#include "networkconnection.h"
#include "assert.h"
#include <stdint.h>
#include <enet\enet.h>

bool NetworkConnection::IsConnected() const
{
	assert(0, "not implemented!");
	return m_Socket != nullptr;
}

void NetworkConnection::DisconnectSocket()
{
	assert(0, "not implemented!");
}

void NetworkConnection::EstablishSocket(const char* hostname, uint16_t port)
{
	(void)hostname;
	(void)port;

	m_OwnSocket = true;
}

void NetworkConnection::UseSocket(void*)
{
	m_OwnSocket = false;
}