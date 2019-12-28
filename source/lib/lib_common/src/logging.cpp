#ifndef DISABLE_LOGGING
#include "logging.h"
#include <ctime>
#include <cstdarg>
#include <cstdio>

#define LOG_PREFIX_LOG       ""
#define LOG_PREFIX_WARNING   "[Warning] "
#define LOG_PREFIX_ERROR     "[Error]   "
#define LOG_PREFIX_TIMESTAMP "[%H:%M:%S]"

#define VA_TO_BUFFER(buffer, fmt)  \
	va_list argptr; \
	va_start(argptr, fmt); \
	vsnprintf(buffer, sizeof(buffer), fmt, argptr); \
	va_end(argptr);

shared::Logging& shared::Logging::GetInstance()
{
	static shared::Logging s_Instance;
	return s_Instance;
}

const char* shared::Logging::GetTimestamp()
{
	static char s_timestamp[128];
	time_t rawtime;
	struct tm timeinfo;

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	strftime(s_timestamp, 128, LOG_PREFIX_TIMESTAMP, &timeinfo);

	return s_timestamp;
}

void shared::Logging::Log(const char* psFmt, ...)
{
	LineBuffer msg = { '\0' };
	VA_TO_BUFFER(msg, psFmt);

	printf("%s " LOG_PREFIX_LOG " %s\n", GetTimestamp(), msg);
}

void shared::Logging::Warn(const char* psFmt, ...)
{
	LineBuffer msg = { '\0' };
	VA_TO_BUFFER(msg, psFmt);

	printf("%s " LOG_PREFIX_WARNING " %s\n", GetTimestamp(), msg);
}

void shared::Logging::Error(const char* psFmt, ...)
{
	LineBuffer msg = { '\0' };
	VA_TO_BUFFER(msg, psFmt);

	printf("%s " LOG_PREFIX_ERROR " %s\n", GetTimestamp(), msg);
}

void shared::Logging::Raw(const char* psFmt, ...)
{
	LineBuffer msg = { '\0' };
	VA_TO_BUFFER(msg, psFmt);

	printf("%s", msg);
}

#endif