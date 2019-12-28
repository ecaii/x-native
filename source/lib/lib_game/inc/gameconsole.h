#pragma once
#include "hash.h"
#include <unordered_map>

namespace shared
{
	class IConsoleDevice;

	class GameConsole
	{
	public:
		typedef void(*ArgumentFunction)(int, int, char**);

		struct ConsoleConfiguration
		{
			ConsoleConfiguration() :
				createNativeWindow(false),
				acceptInput(false),
				bindKillSignal(false)
			{ }

			bool createNativeWindow;
			bool acceptInput;
			bool bindKillSignal;
		};

		GameConsole(const ConsoleConfiguration& config);

		//
		// Core
		//
		void Initialize(int argc, char** argv);
		void Shutdown();
		bool HasKillSignal() const;

		//
		// Helpers
		//
		void Pause();

		//
		// Arguments
		//
		void RegisterArgument(const CompilerHashU8& hash, ArgumentFunction fn)
		{
			m_ArgumentFunctions[hash.GetValue()] = fn;
		}

		void ExecuteArgument(const char* argument, int argi, int argc, char** argv);

	protected:
		IConsoleDevice*      m_pDevice;
		ConsoleConfiguration m_Config;

		std::unordered_map<CompilerHashU8::Value, ArgumentFunction> m_ArgumentFunctions;
	};
}
