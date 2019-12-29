#include "networkmanager.h"
#include "gamejobsystem.h"
#include "gamehighrestimer.h"

#include <enet\enet.h>

void NetworkManager::Initialize(const NetworkManagerConfiguration& config)
{
	if (config.synchronisationMaxSize == 0) {
		assert_crash(0, "config.synchronisationMaxSize == 0");
		return;
	}

	if (enet_initialize() != 0) {
		assert_crash(0, "enet.initialize() failed");
		return;
	}

	m_pConnectionLayer = CreateConnectionLayer(this);

	m_ManagerState = ManagerState::READY;
	m_pSynchronisation = new NetworkSynchronisation(config.synchronisationMaxSize);

	NetworkObject::InitializeGlobalManifest();
}

void NetworkManager::Shutdown()
{
	// Kill all connections before deinitializing enet, nvmp legacy did this and it seemed to work
	m_aConnections.clear();

	// Destroy our connection layer
	DestroyConnectionLayer();

	enet_deinitialize();

	m_ManagerState = ManagerState::INACTIVE;
	delete m_pSynchronisation;
	m_pSynchronisation = nullptr;
}

void NetworkManager::RunJobs()
{
	GameJobSystem::GetInstance().Start(shared::CompilerHashU8("NetworkObjectJobUpdate"));

	for (auto&& element : m_aObjects)
	{
		NetworkObject* object = element.second.get();
		if (object->ShouldUpdate())
		{
			object->PreJobUpdate();
			GameJobSystem::GetInstance().Execute([object] { object->MainJobUpdate(); });
		}
	}

	GameJobSystem::GetInstance().Wait();
}

void NetworkManager::Update()
{
	RunJobs();
	Synchronise();

	// Ensure the layer is consistently kept up to date
	NetworkConnectionLayer& layer = GetConnectionLayer();
	layer.Update();
}

void NetworkManager::Synchronise()
{
	NetworkSynchronisation& sync = GetPrimarySynchronisation();

	// Synchronise instance indicies read if client, write if server
	switch (sync.GetMode())
	{
	case NetworkSynchronisation::Mode::SYNC_MODE_READ:
		if (IsClient())
		{
			NetworkObject::SynchroniseInstanceIndicies(sync);
		}
		break;
	case NetworkSynchronisation::Mode::SYNC_MODE_WRITE:
		if (IsDedicatedServer())
		{
			NetworkObject::SynchroniseInstanceIndicies(sync);
		}
		break;
	default:;
	}

	// For every bit, check if we need to create or delete the index

	// Synchronise all objects to all connections
	for (auto&& conpair : m_aConnections)
	{
		NetworkConnection* connection = conpair.second.get();

		for (auto&& netpair : m_aObjects)
		{
			NetworkObject* object = netpair.second.get();
			if (object->ShouldSynchronise(*connection))
			{
				object->Synchronise(sync);
			}
		}
	}
}

void NetworkManager::SynchroniseIn()
{
}
