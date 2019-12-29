#pragma once
#include "NetworkSynchronisation.h"

// This is a clever bit state so that we can see an identifier, and identify if it is a server. When we synchronise client to server with this ID we just
// cull the final bit to ensure security, but 
#define NETWORK_IDENTIFIER_BIT_SERVER   0x40000000

#define NETWORK_CONNECTION_MAX  32
#define NETWORK_CONNECTION_ID_INVALID (NetworkConnectionIdentifier)(-1)

// The server ID doesn't need any mutation, just the bit set, it's magnifique
#define NETWORK_CONNECTION_ID_SERVER  (NetworkConnectionIdentifier)(NETWORK_IDENTIFIER_BIT_SERVER)

class NetworkConnectionIdentifier : public NetworkIdentifier<NETWORK_CONNECTION_MAX, 2>
{
public:
	NetworkConnectionIdentifier(uint32_t id)
		: NetworkIdentifier(id)
	{
	}

	bool      IsDedicatedServer() const { return (GetIdentifier() & NETWORK_IDENTIFIER_BIT_SERVER) != 0; }
	bool      IsClient() const { return (GetIdentifier() & NETWORK_IDENTIFIER_BIT_SERVER) == 0; }
	void      SetIsServer(bool value) { SetStateBit(NETWORK_IDENTIFIER_BIT_SERVER, value); }
};

//
// NetworkConnection is a single connection to a peer, could be a client or a server, it's all the same
//
class NetworkConnection
{
public:
	NetworkConnection(NetworkConnectionIdentifier id/* ENETPeer */) :
		m_ID(id),
		m_Socket(nullptr),
		m_OwnSocket(false)
	{
	}

	~NetworkConnection()
	{
		if (IsConnected())
		{
			DisconnectSocket();
		}
	}

	bool       IsConnected()       const;
	bool       IsClient()          const { return  m_ID.IsClient(); }
	bool       IsDedicatedServer() const { return !m_ID.IsClient(); }

	NetworkConnectionIdentifier GetID() const { return m_ID; }

	void       EstablishSocket(const char* hostname, uint16_t port);
	void       UseSocket(void*);
	void       DisconnectSocket();

	//
	//  Receive and Send use byte operations on the synchronisation buffer to inflate and deflate the contents to the buffers
	//
	void       Receive(NetworkSynchronisation& sync);
	void       Send(const NetworkSynchronisation& sync);

protected:
	NetworkConnectionIdentifier m_ID;
	void*                       m_Socket;
	bool                        m_OwnSocket;
};
