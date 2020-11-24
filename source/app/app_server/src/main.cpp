#include "unittests.h"
#include "networksynchronisation.h"
#include "networkmanagerdedicatedserver.h"
#include "objects\networkactorobject.h"

#include "gamehighrestimer.h"
#include "gameconsole.h"
#include "gamesettings.h"
#include <thread>

GameSettings::List g_ServerSettings =
{
	// Network configuration
	{ "Network",   "Hostname", "*" },
	{ "Network",   "Port",     27015 },
	{ "Network",   "MaxPeers", 32 },

	// Personalisation
	{ "Server",   "DisplayName",     "NVMPX Default Server" },
	{ "Server",   "Hostname",        "localhost" },

	// SDK configuration
	{ "SDK",     "InitScript", "init.lua" }
};

bool g_ServerProcRunOnce = false;

class GameDedicatedServer : public NetworkManagerDedicatedServer
{
public:
	GameDedicatedServer() :
		NetworkManagerDedicatedServer()
	{
	}

	void Initialize();
};

void GameDedicatedServer::Initialize()
{
	NetworkManager::NetworkManagerConfiguration config;
	config.synchronisationMaxSize = NETWORK_SYNCHRONISATION_MAX_SIZE;

	// Register network objects
	NetworkObject::RegisterNetworkObjectManifest<NetworkActorObject>(NETWORK_OBJECT_ID_ACTOR, NETWORK_MAX_ACTORS);

	// Send over the config
	NetworkManagerDedicatedServer::Initialize(config);
}

int main(int argc, char** argv)
{
	GameHighResTimer::Reset();

	GameConsole::ConsoleConfiguration consoleConfig;
	consoleConfig.createNativeWindow = false;
	consoleConfig.acceptInput = true;
	consoleConfig.bindKillSignal = true;

	GameConsole console(consoleConfig);

#if _DEBUG
	console.RegisterArgument(shared::CompilerHashU8("-test"), [](int, int, char**)
	{
	});
#endif
	console.RegisterArgument(shared::CompilerHashU8("-run_once"), [](int, int, char**)
	{
		g_ServerProcRunOnce = true;
	});

	console.Initialize(argc, argv);

#if _DEBUG
	///
	/// Do UnitTests beforehand
	///
	TESTABLE_REGISTER(NetworkSynchronisation);
	TESTABLE_REGISTER(NetworkConnectionIdentifier);

	if (!TESTABLE_RUN_ALL())
	{
		console.Pause();
		exit(0);
	}
#endif

	GameDedicatedServer server;
	server.Initialize();

	GameSettings settings("server.cfg", &g_ServerSettings);

	const char*    bind_hostname = settings.Get("Network", "Hostname")->GetString();
	const uint16_t bind_port = settings.Get("Network", "Port")->GetNumericAs<uint16_t>();

	// Accept any hostname if asterisk is passed.
	if (strlen(bind_hostname) == 0 || bind_hostname[0] == '*')
		bind_hostname = nullptr;

	server.Listen(bind_hostname, bind_port);

	while (server.IsActive() && !console.HasKillSignal())
	{
		server.Update();
		GameHighResTimer::Reset();

		if (g_ServerProcRunOnce)
		{
			break;
		}
	}

	DbgLog("Shutting down ...");
	server.Shutdown();
	console.Shutdown();
	return 0;
}