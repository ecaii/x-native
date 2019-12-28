#pragma once
#ifdef _WIN32
#include "consoledevices\consoledevice.h"

class ConsoleDeviceWindows : public IConsoleDevice
{
public:
	ConsoleDeviceWindows() { }

	virtual void CreateConsoleWindow();
	virtual void DestroyConsoleWindow();
	virtual void BindKillSignal();
	virtual bool HasKillSignal() const { return m_KillSignal; }

	static bool m_KillSignal;
};

#endif 