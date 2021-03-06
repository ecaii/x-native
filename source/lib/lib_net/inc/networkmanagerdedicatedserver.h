#pragma once
#include "networkmanager.h"

class NetworkManagerDedicatedServer : public NetworkManager
{
public:
	NetworkManagerDedicatedServer()
		: NetworkManager()
	{
	}

	//
	// Boilerplate
	//
	virtual bool IsDedicatedServer() const {
		return true;
	}

	virtual bool IsClient() const {
		return false;
	}

	virtual void Initialize(const NetworkManagerConfiguration& config);
	virtual void Shutdown();

	virtual NetworkConnectionLayer* CreateConnectionLayer(NetworkManager* pNetworkManager) const;

	//
	// Server Goodies
	//
	void Listen(const char* psHostname, uint16_t port);
};
