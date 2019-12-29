#pragma once
#include "networkobject.h"
#include "networkshared.h"

class NetworkActorObject : public NetworkObject
{
public:
	NETWORK_OBJECT_MANIFEST(NetworkActorObject, NETWORK_OBJECT_ID_ACTOR);
};