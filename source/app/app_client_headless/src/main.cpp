#include "networksynchronisation.h"
#include "networkmanagerclient.h"
#include "objects\networkactorobject.h"

#include "gamehighrestimer.h"
#include "gameconsole.h"
#include "gamesettings.h"
#include <thread>

class GameClient : public NetworkManagerClient
{
public:
	GameClient() :
		NetworkManagerClient()
	{
	}

	void Initialize();
};

void GameClient::Initialize()
{
	NetworkManager::NetworkManagerConfiguration config;
	config.synchronisationMaxSize = NETWORK_SYNCHRONISATION_MAX_SIZE;

	// Register network objects
	NetworkObject::RegisterNetworkObjectManifest<NetworkActorObject>(NETWORK_OBJECT_ID_ACTOR, NETWORK_MAX_ACTORS);

	// Send over the config
	NetworkManagerClient::Initialize(config);
}

int main(int argc, char** argv)
{
	GameHighResTimer::Reset();

	GameConsole::ConsoleConfiguration consoleConfig;
	consoleConfig.createNativeWindow = false;
	consoleConfig.acceptInput = true;
	consoleConfig.bindKillSignal = true;

	GameConsole console(consoleConfig);
	console.Initialize(argc, argv);

	GameClient client;
	client.Initialize();

	// Connect to the localhost server
	client.Connect(NETWORK_DEFAULT_HOSTNAME, NETWORK_DEFAULT_PORT);

	// Shutdown if the network manager becomes inactive (internal shutdown), or loses connection (kicked or socket timeout), or if the console gets a kill signal
	while (client.IsActive() && client.IsConnected() && !console.HasKillSignal())
	{
		client.Update();
		GameHighResTimer::Reset();
	}

	DbgLog("Shutting down client ...");
	client.Shutdown();
	console.Pause();
	console.Shutdown();
	return 0;
}