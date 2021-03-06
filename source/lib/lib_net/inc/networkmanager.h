#pragma once
#include "networkconnection.h"
#include "networkobject.h"
#include "gamejobsystem.h"
#include <unordered_map>

class NetworkManager;

//
// An intermediate layer between the metwork manager, and the connection library (ENet) without too much
// complicated abstraction
//
class NetworkConnectionLayer
{
public:
	NetworkConnectionLayer(NetworkManager* /*pNetworkManager*/) { }
	virtual ~NetworkConnectionLayer() { }

	virtual void CreateHost() { assert(0, "do not call base"); };
	virtual void DestroyHost() { assert(0, "do not call base"); }
	
	virtual std::shared_ptr<NetworkConnection> Connect(const char* /*hostname*/, uint16_t /*portname*/) { assert(0, "Connect() not implemented!"); return nullptr; };
	virtual void                               Listen(const char* /*hostname*/, uint16_t /*portname*/)  { assert(0, "Listen() not implemented!"); };
	virtual void                               Disconnect() { assert(0, "not implemented!"); };
	virtual void                               Update() = 0;

	virtual uint32_t GetHostName() = 0;
	virtual uint16_t GetPortName() = 0;
};

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
		m_pConnectionLayer = nullptr;
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

	virtual NetworkConnectionLayer* CreateConnectionLayer(NetworkManager* pNetworkManager) const = 0;
	virtual void                    DestroyConnectionLayer()
	{
		delete m_pConnectionLayer;
		m_pConnectionLayer = nullptr;
	}

	void         Update();
	void         RunJobs();
	void         Synchronise();
	void         SynchroniseIn();

protected:
	std::unordered_map<NetworkConnectionIdentifier::UniqueID, std::shared_ptr<NetworkConnection>>  m_aConnections;
	std::unordered_map<NetworkObjectIdentifier, std::shared_ptr<NetworkObject>>                    m_aObjects;

	NetworkSynchronisation&   GetPrimarySynchronisation() { assert_crash(m_pSynchronisation && IsActive(), "get primary synchronisation not safe"); return *m_pSynchronisation; }
	NetworkConnectionLayer&   GetConnectionLayer()        { assert_crash(m_pConnectionLayer && IsActive(), "get connection layer not safe"); return *m_pConnectionLayer; }

	ManagerState GetState() const
	{
		return m_ManagerState;
	}

private:
	ManagerState               m_ManagerState;
	NetworkSynchronisation*    m_pSynchronisation;
	NetworkConnectionLayer*    m_pConnectionLayer;
};
