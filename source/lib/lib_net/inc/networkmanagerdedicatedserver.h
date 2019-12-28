#pragma once
#include "networkmanager.h"

class NetworkManagerDedicatedServer : public NetworkManager
{
public:
	NetworkManagerDedicatedServer()
		: NetworkManager()
	{
	}

	virtual bool IsDedicatedServer() const {
		return true;
	}

	virtual bool IsClient() const {
		return false;
	}

	virtual void Initialize(const NetworkManagerConfiguration& config);
	virtual void Shutdown();
};
