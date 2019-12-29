#pragma once
#include "networkmanager.h"

class NetworkManagerClient : public NetworkManager
{
public:
	NetworkManagerClient()
		: NetworkManager()
	{
	}

	//
	// Boilerplate
	//
	virtual bool IsDedicatedServer() const {
		return false;
	}

	virtual bool IsClient() const {
		return true;
	}

	virtual void Initialize(const NetworkManagerConfiguration& config);
	virtual void Shutdown();

	virtual NetworkConnectionLayer* CreateConnectionLayer(NetworkManager* pNetworkManager) const;

	//
	// Client Goodies
	//
	bool IsConnected() const;
	bool Connect(const char* psHostname, uint16_t port);
};
