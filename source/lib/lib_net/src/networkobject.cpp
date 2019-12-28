#include "networkobject.h"
#include "networksynchronisation.h"
#include "networkconnection.h"

uint32_t s_currentID = 0;
NetworkObject::ObjectManifest* NetworkObject::ms_pManifests = nullptr;

void NetworkObject::SynchroniseInstanceIndicies(NetworkSynchronisation& sync)
{
	ObjectManifest* pManifest = ms_pManifests;
	while (pManifest)
	{
		// Make the shadow bytes the same as the active bytes, then synchronise into the shadow
		pManifest->ResetShadow();

		for (uint32_t i = 0; i < pManifest->maxInstances; ++i)
		{
			const uint32_t index = i / NETWORK_BYTE_SIZE;
			const uint32_t offset = i % NETWORK_BYTE_SIZE;

			bool instanceExists = (pManifest->instanceIndiciesShadow[index] & (1 << offset)) != 0;
			sync.Synchronise(instanceExists, 1);

			if (instanceExists)
			{
				pManifest->instanceIndiciesShadow[index] |= (1 << offset);
			}
			else
			{
				pManifest->instanceIndiciesShadow[index] &= ~(1 << offset);
			}
		}

		// Write shadow to primary
		pManifest = pManifest->next;
	}
}

const char* NetworkObject::GetInstanceIndicies(NetworkObjectTypeIdentifier typeID)
{
	ObjectManifest* pManifest = GetObjectManifest(typeID);
	if (pManifest)
	{
		pManifest->instanceIndicies;
	}

	return nullptr;
}

NetworkObject::ObjectManifest* NetworkObject::GetObjectManifest(NetworkObjectTypeIdentifier type)
{
	ObjectManifest* pManifest = ms_pManifests;
	while (pManifest && pManifest->next) {
		if (pManifest->typeID == type) {
			return pManifest;
		}
		pManifest = pManifest->next;
	}

	return nullptr;
}

void NetworkObject::InitializeGlobalManifest()
{
	assert(0, "not implemented!");
}

bool NetworkObject::ShouldSynchronise(NetworkConnection& connection) const
{
	return connection.IsConnected() && IsLocal();
}

void NetworkObject::Synchronise(NetworkSynchronisation& sync)
{
	// All connected peers need to know of the object owner as synchronised by the owner
	sync.Synchronise(m_Owner, 32);

	// Iterate over all network variables, if the frame requests just deltas then we just send deltas to the frame
}

void NetworkObject::SetServerCurrentID(uint32_t id)
{
	s_currentID = id;
	DbgLog("Setting server current object ID to %u", id);
}

NetworkObject::NetworkObject() :
	m_Owner(NETWORK_OBJECT_OWNER_NULL)
{
	m_ID = (s_currentID++);
}

NetworkObject::NetworkObject(NetworkObjectIdentifier id, NetworkConnectionIdentifier owner) :
	m_Owner(owner)
{
	m_ID = id;
}

#if _DEBUG
const char* NetworkObject::GetName() const
{
	static char sName[128];
	snprintf(sName, sizeof(sName), "%u<%u>", GetType(), m_ID);
	return sName;
}
#endif 