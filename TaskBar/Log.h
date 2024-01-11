#pragma once

class Log {
public:
	enum LogLevel_T {
		LOG_ERROR,
		LOG_WARN,
		LOG_INFO,
		LOG_DEBUG,
	};

	static void Debug(const wchar_t* format, ...);
	static void Info(const wchar_t* format, ...);
	static void Warn(const wchar_t* format, ...);
	static void Error(const wchar_t* format, ...);
	static void SetLogLevel(LogLevel_T level);

private:
	static const wchar_t* APP_NAME;
	static LogLevel_T LogLevel;
	static void LogMessage(LogLevel_T level, const wchar_t* format, ...);
	const static wchar_t* GetLogLevelString(LogLevel_T);
};