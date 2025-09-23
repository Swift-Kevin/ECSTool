#include "ConsoleLog.h"

namespace LOG
{
	entt::registry* p_registry = nullptr;

	std::string EnumToLabel(LogSeverity _severity)
	{
		std::string retval = "";
		switch (_severity)
		{
		case LOG::LogSeverity::Log:
			retval = "Log";
			break;
		case LOG::LogSeverity::Warning:
			retval = "Warning";
			break;
		case LOG::LogSeverity::Error:
			retval = "Error";
			break;
		case LOG::LogSeverity::Debug:
			retval = "Debug";
			break;
		default:
			break;
		}

		return retval;
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
}