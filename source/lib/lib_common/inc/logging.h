#pragma once
#ifndef DISABLE_LOGGING
	namespace shared
	{
		class Logging
		{
		public:
			typedef char LineBuffer[512];

			enum class LogType
			{
				kLogType_Log,
				kLogType_Warning,
				kLogType_Error
			};

			void Log(const char* psFmt, ...);
			void Warn(const char* psFmt, ...);
			void Error(const char* psFmt, ...);
			void Raw(const char* psFmt, ...);

			static Logging&      GetInstance();
			static const char*   GetTimestamp();

		private:
			const char*  m_psFileName;
		};
	}

	#define DbgInvoke(method, psFmt, ...) { shared::Logging::GetInstance().##method##(psFmt, ##__VA_ARGS__); }
	#define DbgLog(psFmt, ...)    DbgInvoke(Log,   psFmt, ##__VA_ARGS__)
	#define DbgWarn(psFmt, ...)   DbgInvoke(Warn,  psFmt, ##__VA_ARGS__)
	#define DbgError(psFmt, ...)  DbgInvoke(Error, psFmt, ##__VA_ARGS__)
	#define DbgLogRaw(psFmt, ...) DbgInvoke(Raw, psFmt, ##__VA_ARGS__)
#elif
	#define DbgLog(psFmt, ...)
	#define DbgWarn(psFmt, ...)
	#define DbgError(psFmt, ...)
	#define DbgLogRaw(psFmt, ...)
#endif 