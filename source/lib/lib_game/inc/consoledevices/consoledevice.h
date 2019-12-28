#pragma once

namespace shared
{
	class IConsoleDevice
	{
	public:
		IConsoleDevice() { };
		virtual ~IConsoleDevice() { };

		//----------------------------------------------
		// Initialisation methods for managing the 
		// hardware handle to the window.
		//----------------------------------------------
		virtual void CreateConsoleWindow() = 0;
		virtual void DestroyConsoleWindow() = 0;
		virtual void BindKillSignal() = 0;
		virtual bool HasKillSignal() const = 0;
	};
}