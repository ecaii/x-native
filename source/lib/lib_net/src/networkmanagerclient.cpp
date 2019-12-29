#include "networkmanagerclient.h"
#include "networkconnection.h"
#include "networkshared.h"
#include <enet\enet.h>

//
// A connection implementation towards a server
//
class NetworkConnectionServer : public NetworkConnection
{
public:
	NetworkConnectionServer(NetworkConnectionIdentifier id) :
		NetworkConnection(id), m_ServerPeer(nullptr), m_Connected(false) { }

	virtual void       UseSocket(void* peer)
	{
		m_ServerPeer = static_cast<ENetPeer*>(peer);
	}

	virtual void       DisconnectSocket()
	{
		enet_peer_reset(m_ServerPeer);
		m_ServerPeer = nullptr;
		m_Connected = false;
	}

	virtual bool       IsConnected()       const
	{
		return m_Connected;
	}

protected:
	ENetPeer* m_ServerPeer;
	bool      m_Connected;
};

//
// A network layer implementation for a client connecting to servers
//
class NetworkConnectionLayerClient : public NetworkConnectionLayer
{
public:
	NetworkConnectionLayerClient(NetworkManager* pNetworkManager) :
		NetworkConnectionLayer(pNetworkManager), m_Host(nullptr) { }

	virtual ~NetworkConnectionLayerClient()
	{
		Disconnect();
	}

	//
	// Host
	//
	virtual void CreateHost()
	{
		m_Host = enet_host_create(nullptr, 1, ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT, 0, 0);
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
		m_ServerConnection.reset();
	}

	//
	// Addressing
	//
	virtual std::shared_ptr<NetworkConnection> Connect(const char* hostname, uint16_t portname)
	{
		CreateHost();

		enet_address_set_host(&m_Address, hostname);
		m_Address.port = portname;

		ENetPeer* peer = enet_host_connect(m_Host, &m_Address, NETWORK_LAYER_MAX_CHANNELS, 0);
		if (!peer) {
			return false;
		}

		// Attempt to connect to the peer before we set it up as a wrapped connection
		if (enet_host_service(m_Host, &m_LastEvent, NETWORK_LAYER_TIMEOUT) > 0 &&
			m_LastEvent.type == ENET_EVENT_TYPE_CONNECT)
		{
			// Create a network connection wrapper for our new peer
			std::shared_ptr<NetworkConnectionServer> result(new NetworkConnectionServer(NETWORK_CONNECTION_ID_SERVER));
			result->UseSocket(peer);

			// Soft reference, if the layer loses connection then we need to knock the connection object offline
			m_ServerConnection = result;

			return result;
		}
		
		Disconnect();
		return nullptr;
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
			switch (m_LastEvent.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					DbgLog("NetworkConnectionLayerClient::Update Connected");
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					if (std::shared_ptr<NetworkConnection> connection = m_ServerConnection.lock())
					{
						DbgLog("NetworkConnectionLayerClient::Update Lost connection");
						connection->DisconnectSocket();
					}
					break;
				}
				default:;
			}
		}
	}

protected:
	ENetAddress m_Address;
	ENetHost*   m_Host;
	ENetEvent   m_LastEvent;

	std::weak_ptr<NetworkConnectionServer> m_ServerConnection;
};

void NetworkManagerClient::Initialize(const NetworkManagerConfiguration& config)
{
	NetworkManager::Initialize(config);
}

void NetworkManagerClient::Shutdown()
{
	NetworkManager::Shutdown();
}

NetworkConnectionLayer* NetworkManagerClient::CreateConnectionLayer(NetworkManager* pNetworkManager) const
{
	return new NetworkConnectionLayerClient(pNetworkManager);
}

bool NetworkManagerClient::IsConnected() const
{
	return m_aConnections.size() != 0 &&
		m_aConnections.find(NETWORK_CONNECTION_ID_SERVER) != m_aConnections.end();
}

bool NetworkManagerClient::Connect(const char* psHostname, uint16_t port)
{
	assert(!IsConnected(), "connecting to host whilst already connected");
	m_aConnections.clear();

	NetworkConnectionLayer& layer = GetConnectionLayer();

	// Establish a new connection for the server
	DbgLog("NetworkManagerClient :: Connecting to %s:%u...", psHostname, port);
	if (std::shared_ptr<NetworkConnection> pNetworkConnection = layer.Connect(psHostname, port))
	{
		assert(pNetworkConnection->IsConnected(), "connected but not state_connected");
		assert(pNetworkConnection->GetID().IsDedicatedServer(), "server connection is not identified as such");

		// Move the connection onto the connection list for processing
		m_aConnections[NETWORK_CONNECTION_ID_SERVER] = std::move(pNetworkConnection);
		return true;
	}
	

	// std::unique_ptr<NetworkConnection> serverConnection = std::make_unique<NetworkConnection>(NETWORK_CONNECTION_ID_SERVER);
	// serverConnection->EstablishSocket(psHostname, port);

	// assert_crash(serverConnection->GetID().IsDedicatedServer(), "server connection is not identified as such");

	// if (serverConnection->IsConnected())
	// {
	//	m_aConnections[NETWORK_CONNECTION_ID_SERVER] = std::move(serverConnection);
	//	return true;
	//}

	return false;
}