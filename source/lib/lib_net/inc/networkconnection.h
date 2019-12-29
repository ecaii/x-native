#pragma once
#include "NetworkSynchronisation.h"

#define NETWORK_IDENTIFIER_BIT_SERVER   1    // Which bit of the identifier represents this connection belongs to the server
#define NETWORK_CONNECTION_MAX          32

// The invalid ID represented as an impossible connection
#define NETWORK_CONNECTION_ID_INVALID   (NetworkConnectionIdentifier)(-1)

// The server ID doesn't need any mutation, just the bit set, it's magnifique - this represents the major dedicated server
#define NETWORK_CONNECTION_ID_SERVER     NetworkConnectionIdentifier(0, NETWORK_IDENTIFIER_BIT_SERVER)

class NetworkConnectionIdentifier : public NetworkIdentifier<NETWORK_CONNECTION_MAX, 1>
{
public:
	NetworkConnectionIdentifier(uint32_t identifier)
		: NetworkIdentifier(identifier)
	{
	}

	NetworkConnectionIdentifier(UniqueID id, uint32_t bits)
		: NetworkIdentifier(id, bits)
	{
	}

	TESTABLE_OBJECT(NetworkConnectionIdentifier);

	bool      IsDedicatedServer() const { return IsBitSet(NETWORK_IDENTIFIER_BIT_SERVER); }
	bool      IsClient() const { return !IsDedicatedServer(); }
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

	virtual ~NetworkConnection();

	bool       IsClient()          const { return  m_ID.IsClient(); }
	bool       IsDedicatedServer() const { return !m_ID.IsClient(); }

	NetworkConnectionIdentifier GetID() const { return m_ID; }

	virtual void       UseSocket(void*)          { assert(0, "do not call base"); }
	virtual void       DisconnectSocket()        { assert(0, "do not call base"); }
	virtual bool       IsConnected()       const { assert(0, "do not call base"); return false; }

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
