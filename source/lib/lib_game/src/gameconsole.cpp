#include "gameconsole.h"
#include "logging.h"
#include "consoledevices\consoledevice.h"
#include "consoledevices\consoledevicewindows.h"

shared::GameConsole::GameConsole(const ConsoleConfiguration& config) :
	m_Config(config),
	m_pDevice(nullptr)
{
#if _WIN32
	m_pDevice = new ConsoleDeviceWindows();
#elif __linux__
	m_pDevice = new ConsoleDeviceLinux();
#else
#error No supported native console type
#endif
}

void shared::GameConsole::ExecuteArgument(const char* argument, int argi, int argc, char** argv)
{
	const CompilerHashU8 hash = CompilerHashU8(argument);
	if (m_ArgumentFunctions.find(hash.GetValue()) != m_ArgumentFunctions.end())
	{
		m_ArgumentFunctions[hash.GetValue()](argi, argc, argv);
	}
	else
	{
		DbgError("Argument '%s' not registered", argument);
	}
}

bool shared::GameConsole::HasKillSignal() const
{
	return m_pDevice->HasKillSignal();
}

void shared::GameConsole::Pause()
{
	// Do not pause if a kill signal is processed, kill means KILL >:(
	if (!HasKillSignal())
	{
		DbgLog("Press any key to continue ...");
		getchar();
	}
}

void shared::GameConsole::Initialize(int argc, char** argv)
{
	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			ExecuteArgument(argv[i], i, argc, argv);
		}
	}

	if (m_Config.createNativeWindow)
	{
		m_pDevice->CreateConsoleWindow();
	}
	if (m_Config.bindKillSignal)
	{
		m_pDevice->BindKillSignal();
	}
}

void shared::GameConsole::Shutdown()
{
	if (m_Config.createNativeWindow)
	{
		m_pDevice->DestroyConsoleWindow();
	}

	delete m_pDevice;
}
