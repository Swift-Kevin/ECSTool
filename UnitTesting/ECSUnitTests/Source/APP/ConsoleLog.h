#ifndef LOG_H
#define LOG_H
#include "../UTIL/ComponentReflection.h"

namespace LOG
{
	enum class LogSeverity : byte { Debug, Warning, Error};
	static std::string EnumToLabel(LogSeverity _severity)
	{
		std::string retval = "";
		switch (_severity)
		{
		case LOG::LogSeverity::Debug:
			retval = "[Debug] : ";
			break;
		case LOG::LogSeverity::Warning:
			retval = "[Warning] : ";
			break;
		case LOG::LogSeverity::Error:
			retval = "[Error] : ";
			break;
		default:
			break;
		}

		return retval;
	}

	bool ShowMessageStatus(bool debug, bool warning, bool error, LogSeverity severity);

	struct LogEntry
	{
		LogSeverity severity = LogSeverity::Debug;
		std::string content = "";

		std::string Message() { return EnumToLabel(severity) + " " + content; }
	};

	struct Logs
	{
		std::vector<LogEntry> messages;
	};

	void InitializeLogSystem(entt::registry& registry);

	void Log(const char* _log, LogSeverity _severity = LOG::LogSeverity::Debug);
	void LogError(const char* _log);
	void LogWarning(const char* _log);
	void LogDebug(const char* _log);
};


// End of LOG_H
#endif