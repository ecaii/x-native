#pragma once
#include "networkconnection.h"
#include "networkobject.h"
#include "gamejobsystem.h"
#include <unordered_map>
class NetworkObject;

class NetworkManager
{
public:
	enum class ManagerState
	{
		INACTIVE,
		READY,
		RUNNING_JOBS,
		SYNCHRONISING
	};

	struct NetworkManagerConfiguration
	{
		uint32_t   synchronisationMaxSize = 0;
	};

	NetworkManager()
	{
		m_ManagerState = ManagerState::INACTIVE;
		m_pSynchronisation = nullptr;
	}

	virtual ~NetworkManager()
	{
		if (IsActive())
		{
			Shutdown();
		}
	}

	virtual void Initialize(const NetworkManagerConfiguration& config);
	virtual void Shutdown();

	virtual bool IsDedicatedServer() const = 0;
	virtual bool IsClient() const = 0;
	virtual bool IsActive() const { return m_ManagerState != ManagerState::INACTIVE; };

	void         Update();
	void         RunJobs();
	void         Synchronise();
	void         SynchroniseIn();

protected:
	std::unordered_map<NetworkConnectionIdentifier::UniqueID, std::unique_ptr<NetworkConnection>>  m_aConnections;
	std::unordered_map<NetworkObjectIdentifier, std::unique_ptr<NetworkObject>>      m_aObjects;

	NetworkSynchronisation&   GetPrimarySynchronisation() { assert_crash(m_pSynchronisation && IsActive(), "get primary synchronisation not safe"); return *m_pSynchronisation; }

	ManagerState GetState() const
	{
		return m_ManagerState;
	}

private:
	ManagerState               m_ManagerState;
	NetworkSynchronisation*    m_pSynchronisation;
};
