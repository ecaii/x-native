#pragma once
#include "networkobject.h"
#include "networkshared.h"

class GameActorNetworkObject : public NetworkObject
{
public:
	NETWORK_OBJECT_MANIFEST(GameActorNetworkObject, GAME_NETWORK_OBJECT_ID_ACTOR);
};