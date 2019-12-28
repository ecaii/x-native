#include "networkmanagerclient.h"
#include "networkconnection.h"

void NetworkManagerClient::Initialize(const NetworkManagerConfiguration& config)
{
	NetworkManager::Initialize(config);
}

void NetworkManagerClient::Shutdown()
{
	NetworkManager::Shutdown();
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

	// Establish a new connection for the server
	std::unique_ptr<NetworkConnection> serverConnection = std::make_unique<NetworkConnection>(NETWORK_CONNECTION_ID_SERVER);
	serverConnection->EstablishSocket(psHostname, port);

	assert_crash(serverConnection->GetID().IsDedicatedServer(), "server connection is not identified as such");

	if (serverConnection->IsConnected())
	{
		// Move the connection onto the connection list for processing
		m_aConnections[NETWORK_CONNECTION_ID_SERVER] = std::move(serverConnection);
		return true;
	}

	return false;
}