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

//# Layer configuration
#define NETWORK_LAYER_MAX_CHANNELS       1
#define NETWORK_LAYER_TIMEOUT            5000

//# Global network object identifiers
enum NetworkObjectIdentifiers
{
	NETWORK_OBJECT_ID_NULL,
	NETWORK_OBJECT_ID_ACTOR,
	NETWORK_OBJECT_ID_WORLD_ITEM,
};
