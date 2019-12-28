#include "networkmanager.h"
#include "gamejobsystem.h"
#include "gamehighrestimer.h"

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
	for (auto&& element : m_aConnections)
	{
		NetworkConnection* connection = element.second.get();

		for (auto&& element : m_aObjects)
		{
			NetworkObject* object = element.second.get();
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

void NetworkManager::Initialize(const NetworkManagerConfiguration& config)
{
	if (config.synchronisationMaxSize == 0) {
		assert_crash(0, "config.synchronisationMaxSize == 0");
		return;
	}

	m_pSynchronisation = new NetworkSynchronisation(config.synchronisationMaxSize);

	NetworkObject::InitializeGlobalManifest();

	m_ManagerState = ManagerState::READY;
}

void NetworkManager::Shutdown()
{
	delete m_pSynchronisation;
	m_pSynchronisation = nullptr;
	m_ManagerState = ManagerState::INACTIVE;
}
