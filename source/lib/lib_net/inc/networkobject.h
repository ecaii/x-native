#pragma once
#include <cinttypes>
#include <vector>

#include "networkconnection.h"
#include "networksynchronisation.h"

class NetworkVariableBase;

typedef uint32_t  NetworkObjectIdentifier;

#define NETWORK_OBJECT_OWNER_SERVER  (-1)
#define NETWORK_OBJECT_OWNER_NULL    (0)
#define NETWORK_OBJECT_MAX           512

#define NETWORK_OBJECT_MANIFEST(TypeName, TypeID) static NetworkObject::ObjectManifest* ObjectManifest; \
	virtual uint32_t GetType() const { return TypeID; }

#define NETWORK_OBJECT_MANIFEST_IMPL(TypeName)    NetworkObject::ObjectManifest* TypeName::ObjectManifest = nullptr;

// Manifest to send for what objects exist
//typedef char NetworkObjectManifestBits[NETWORK_CONNECTION_MAX / NETWORK_BYTE_SIZE];

class NetworkObject
{
public:
	typedef uint32_t   NetworkObjectTypeIdentifier;
#define NETWORK_OBJECT_TYPE_NULL NetworkObjectTypeIdentifier(0)

	struct ObjectManifest
	{
		typedef NetworkObject*(*CreationFunction)();
		typedef void(*DeletionFunction)(NetworkObject*);

		ObjectManifest() :
			size(0),
			maxInstances(0),
			instanceIndicies(nullptr),
			instanceIndiciesShadow(nullptr),
			deletion(nullptr),
			creation(nullptr),
			typeID(NETWORK_OBJECT_TYPE_NULL),
			next(nullptr)
		{ }

		~ObjectManifest()
		{
			delete instanceIndicies;
			delete instanceIndiciesShadow;
		}

		void CreateInstanceIndicies()
		{
			instanceIndicies = new char[maxInstances / NETWORK_BYTE_SIZE];
			instanceIndiciesShadow = new char[maxInstances / NETWORK_BYTE_SIZE];
			DbgLog("ObjectManifest %u has %u instance bytes (size is %u)", typeID, sizeof(instanceIndicies), sizeof(this));
		}

		void ResetShadow()
		{
			memcpy(instanceIndiciesShadow, instanceIndicies, maxInstances / NETWORK_BYTE_SIZE);
		}

		uint32_t            size;
		uint32_t            maxInstances;
		DeletionFunction    deletion;
		CreationFunction    creation;
		NetworkObjectTypeIdentifier typeID;

		char*               instanceIndicies;
		char*               instanceIndiciesShadow;
		ObjectManifest*     next;
	};

	/// 
	///  Registration
	///
	static ObjectManifest* GetObjectManifest(NetworkObjectTypeIdentifier typeID);
	static void            InitializeGlobalManifest();

	template <class T>
	static void            RegisterNetworkObjectManifest(NetworkObjectTypeIdentifier type, uint32_t maxInstances)
	{
		ObjectManifest* pManifest = new ObjectManifest;
		pManifest->creation = []() -> NetworkObject* { return new T; };
		pManifest->deletion = [](NetworkObject* p) { delete p; };
		pManifest->typeID = type;
		pManifest->size = sizeof(T);
		pManifest->maxInstances = maxInstances;

		// Find the tail
		ObjectManifest* pTail = ms_pManifests;
		while (pTail && pTail->next) {
			pTail = pTail->next;
		}

		if (pTail)
		{
			pTail->next = pManifest;
		}
		else
		{
			ms_pManifests = pManifest;
		}

		T::ObjectManifest = pManifest;
	}

	template <class T>
	static T* CreateNetworkObject()
	{
		ObjectManifest* pManifest = T::ObjectManifest;
		assert_crash(pManifest, "no manifest allocated");

		return pManifest->creation();
	}

	template <class T>
	static void DeleteNetworkObject(void*& p)
	{
		ObjectManifest* pManifest = T::ObjectManifest;
		assert_crash(pManifest, "no manifest allocated");
		pManifest->deletion(p);
		p = nullptr;
	}

	static void        SynchroniseInstanceIndicies(NetworkSynchronisation& sync);
	static const char* GetInstanceIndicies(NetworkObjectTypeIdentifier typeID);

	NetworkObject();
	explicit NetworkObject(NetworkObjectIdentifier id, NetworkConnectionIdentifier owner);
	virtual ~NetworkObject() { }

	virtual uint32_t GetType() const = 0;

#if _DEBUG
	const char* GetName() const;
#endif

	bool                    IsLocal()    const { return m_Owner.IsLocal(); }
	bool                    IsRemote()   const { return !IsLocal(); }
	NetworkObjectIdentifier GetID()      const { return m_ID; }

	virtual void PreJobUpdate() { }
	virtual void MainJobUpdate() { }

	virtual bool ShouldUpdate() const { return true; }
	virtual bool ShouldSynchronise(NetworkConnection& connection) const;

	virtual void Synchronise(NetworkSynchronisation& frame);

	// To keep the current object ID in sync in-case of local creation
	static void SetServerCurrentID(uint32_t id);

	void RegisterVariableReference(NetworkVariableBase* variable)
	{
		m_Variables.push_back(variable);
	}

protected:
	NetworkObjectIdentifier           m_ID;
	NetworkConnectionIdentifier       m_Owner;
	std::vector<NetworkVariableBase*> m_Variables;

	static ObjectManifest* ms_pManifests;
};