#include "networkconnection.h"
#include "assert.h"
#include <stdint.h>

bool NetworkConnection::IsConnected() const
{
	assert(0, "not implemented!");
	return m_Socket != nullptr; // && m_pSocket->AAA;
}

void NetworkConnection::DisconnectSocket()
{
	if (m_Socket)
	{
		//		m_Socket->
	}
}

void NetworkConnection::EstablishSocket(const char* hostname, uint16_t port)
{

}

void NetworkConnection::UseSocket(void*)
{

}