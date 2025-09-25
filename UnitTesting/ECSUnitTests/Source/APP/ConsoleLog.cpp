#include "pch.h"
#include "ConsoleLog.h"

namespace LOG
{
	entt::registry* p_registry = nullptr;

	bool ShowMessageStatus(bool debug, bool warning, bool error, LogSeverity severity)
	{
		switch (severity)
		{
		case LOG::LogSeverity::Debug:   return debug;
		case LOG::LogSeverity::Warning: return warning;
		case LOG::LogSeverity::Error:   return error;
		}

		return false;
	}

	void InitializeLogSystem(entt::registry& registry)
	{
		p_registry = &registry;
	}

	void Log(const char* _log, LogSeverity _severity)
	{
		if (!p_registry)
			return;

		LogEntry entry;
		entry.severity = _severity;
		entry.content = _log;

		auto& logs = p_registry->ctx().get<Logs>();
		logs.messages.push_back(entry);
	}

	void LogWarning(const char* _log)
	{
		Log(_log, LOG::LogSeverity::Warning);
	}

	void LogDebug(const char* _log)
	{
		Log(_log, LOG::LogSeverity::Debug);
	}

	void LogError(const char* _log)
	{
		Log(_log, LOG::LogSeverity::Error);
	}
}