#pragma once

//# Max synchronisation buffer size
#define NETWORK_SYNCHRONISATION_MAX_SIZE 1024

//# Run network unit tests
#ifndef NETWORK_RUN_UNIT_TESTS
	#define NETWORK_RUN_UNIT_TESTS       0
#endif

//# Object limits
#define NETWORK_MAX_ACTORS               255
#define NETWORK_MAX_WORLD_ITEMS          255

//# Global base configuration
#define NETWORK_DEFAULT_HOSTNAME         "localhost"
#define NETWORK_DEFAULT_PORT             27015

//# Global network object identifiers
enum GameNetworkObjectIdentifiers
{
	GAME_NETWORK_OBJECT_ID_NULL,
	GAME_NETWORK_OBJECT_ID_ACTOR,
	GAME_NETWORK_OBJECT_ID_WORLD_ITEM,
};
