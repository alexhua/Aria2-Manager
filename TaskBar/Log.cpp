#include "Log.h"

#include <cstdio>

const wchar_t* Log::APP_NAME = L"Aria2 Manager";

#ifdef _DEBUG
Log::LogLevel_T Log::LogLevel = LOG_DEBUG;
#else
Log::LogLevel_T Log::LogLevel = Log::LOG_INFO;
#endif

void Log::LogMessage(LogLevel_T logType, const wchar_t* format, va_list args) {
	if (logType <= LogLevel) {
		wprintf(L"\n");
		wprintf(L"%ls %ls ", APP_NAME, GetLogLevelString(logType));
		vwprintf(format, args);
		wprintf(L"\n");
	}
}

void Log::Debug(const wchar_t* format, ...) {
	va_list args;
	va_start(args, format);
	LogMessage(LOG_DEBUG, format, args);
	va_end(args);
}

void Log::Info(const wchar_t* format, ...) {
	va_list args;
	va_start(args, format);
	LogMessage(LOG_INFO, format, args);
	va_end(args);
}

void Log::Warn(const wchar_t* format, ...) {
	va_list args;
	va_start(args, format);
	LogMessage(LOG_WARN, format, args);
	va_end(args);
}

void Log::Error(const wchar_t* format, ...) {
	va_list args;
	va_start(args, format);
	LogMessage(LOG_ERROR, format, args);
	va_end(args);
}

void Log::SetLogLevel(const LogLevel_T level) {
	Log::LogLevel = level;
}

const wchar_t* Log::GetLogLevelString(LogLevel_T level) {
	switch (level) {
	case Log::LOG_DEBUG:
		return L"\x1b[34m[DEBUG]\x1b[0m";
	case Log::LOG_INFO:
		return L"\x1b[32m[INFO]\x1b[0m";
	case Log::LOG_WARN:
		return L"\x1b[33m[WARN]\x1b[0m";
	case Log::LOG_ERROR:
		return L"\x1b[31m[ERROR]\x1b[0m";
	default:
		return L"[UNKNOWN]";
	}
}