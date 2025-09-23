#ifndef LOG_H
#define LOG_H
#include "../UTIL/ComponentReflection.h"

namespace LOG
{
	enum class LogSeverity : byte { Log, Warning, Error, Debug };
	static std::string EnumToLabel(LogSeverity _severity);

	struct LogEntry
	{
		LogSeverity severity = LogSeverity::Log;
		std::string content = "";
	};

	struct Logs
	{
		std::vector<LogEntry> messages;
	};

	void InitializeLogSystem(entt::registry& registry);

	void Log(const char* _log, LogSeverity _severity);
};


// End of LOG_H
#endif