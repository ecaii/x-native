#include "networkmanagerdedicatedserver.h"

void NetworkManagerDedicatedServer::Initialize(const NetworkManagerConfiguration& config)
{
	NetworkManager::Initialize(config);
}

void NetworkManagerDedicatedServer::Shutdown()
{
	NetworkManager::Shutdown();
}