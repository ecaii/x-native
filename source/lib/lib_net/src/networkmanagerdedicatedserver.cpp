#include "networkmanagerdedicatedserver.h"
#include "networkconnection.h"
#include "networkshared.h"
#include <enet\enet.h>

//
// A connection implementation towards a single client
//
class NetworkConnectionClient : public NetworkConnection
{
public:
	NetworkConnectionClient(NetworkConnectionIdentifier id)
		: NetworkConnection(id)
		, m_ClientPeer(nullptr)
		, m_Connected(false)
	{ }

	virtual void UseSocket(void* peer)
	{
		m_ClientPeer = static_cast<ENetPeer*>(peer);
	}

	virtual void DisconnectSocket()
	{
		enet_peer_reset(m_ClientPeer);
		m_ClientPeer = nullptr;
		m_Connected = false;
	}

	virtual bool IsConnected() const
	{
		return m_Connected;
	}

protected:
	ENetPeer* m_ClientPeer;
	bool      m_Connected;
};

//
// A network layer implementation for a server
//
class NetworkConnectionLayerServer : public NetworkConnectionLayer
{
public:
	NetworkConnectionLayerServer(NetworkManager* pNetworkManager)
		: NetworkConnectionLayer(pNetworkManager)
		, m_Host(nullptr)
		, m_Address()
		, m_LastEvent()
	{ }

	//
	// Host
	//
	virtual void CreateHost()
	{
		m_Host = enet_host_create(&m_Address, NETWORK_CONNECTION_MAX, ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT, 0, 0);
		if (!m_Host) {
			assert_crash(0, "no enet host created");
			return;
		}
	}

	virtual void DestroyHost()
	{
		enet_host_destroy(m_Host);
		m_Host = nullptr;
	}

	virtual void Disconnect()
	{
		DestroyHost();
	}

	//
	// Addressing
	//
	virtual void Listen(const char* hostname, uint16_t portname)
	{
		if (hostname == nullptr)
		{
			m_Address.host = ENET_HOST_ANY;
		}
		else
		{
			enet_address_set_host(&m_Address, hostname);
		}

		m_Address.port = portname;

		CreateHost();
	}

	virtual uint32_t GetHostName()
	{
		return m_Address.host;
	}

	virtual uint16_t GetPortName()
	{
		return m_Address.port;
	}

	virtual void Update()
	{
		while (m_Host && enet_host_service(m_Host, &m_LastEvent, 0) > 0)
		{
		}
	}

protected:
	ENetAddress m_Address;
	ENetHost*   m_Host;
	ENetEvent   m_LastEvent;
};

void NetworkManagerDedicatedServer::Initialize(const NetworkManagerConfiguration& config)
{
	NetworkManager::Initialize(config);
}

void NetworkManagerDedicatedServer::Shutdown()
{
	NetworkManager::Shutdown();
}

NetworkConnectionLayer* NetworkManagerDedicatedServer::CreateConnectionLayer(NetworkManager* pNetworkManager) const
{
	return new NetworkConnectionLayerServer(pNetworkManager);
}

void NetworkManagerDedicatedServer::Listen(const char* psHostname, uint16_t port)
{
	m_aConnections.clear();

	NetworkConnectionLayer& layer = GetConnectionLayer();
	layer.Listen(psHostname, port);

	DbgLog("NetworkManagerDedicatedServer :: Listening on %s:%u", psHostname, port);
}