#if _WIN32
#include "consoledevices\consoledevice.h"
#include "consoledevices\consoledevicewindows.h"
#include "assert.h"
#include <Windows.h>

bool shared::ConsoleDeviceWindows::m_KillSignal = false;

BOOL WINAPI ConsoleHandler(DWORD winEvent)
{
	switch (winEvent)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		shared::ConsoleDeviceWindows::m_KillSignal = true;
		return TRUE;
	default:
		break;
	}

	return FALSE;
}

void shared::ConsoleDeviceWindows::BindKillSignal()
{
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
	{
		assert_crash(0, "console control handler failed");
	}
}

void shared::ConsoleDeviceWindows::CreateConsoleWindow()
{
	// Bind a console to the process if the bCreateConsole
	// signal isn't false.
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
}

void shared::ConsoleDeviceWindows::DestroyConsoleWindow()
{
	assert(0, "needs implementation");
}

#endif