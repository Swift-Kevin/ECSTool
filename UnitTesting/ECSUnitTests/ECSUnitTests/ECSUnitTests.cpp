#include "pch.h"
#include "CppUnitTest.h"
#include "../Source/CCL.h"
#include "../Source/APP/ConsoleLog.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ECSUnitTests
{
	TEST_CLASS(ECSUnitTests)
	{
	public:

#pragma region Logger Unit Tests

		TEST_METHOD(CHECK_LOGGER_DEBUG)
		{
			/*
				Testing: Can users use both Log and Log Debug
				Verify: Severity and Message are equivalent.
			*/

			entt::registry registry;
			// initialize info
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<LOG::Logs>();
				LOG::InitializeLogSystem(registry);
			}

			std::string logMsg = "Example Log Message - TEST LOG DEBUG LEVEL";
			LOG::Log(logMsg.c_str());
			LOG::LogDebug(logMsg.c_str());
			LOG::Logs& logComponent = registry.ctx().get<LOG::Logs>();

			LOG::LogEntry& baseLogCall = logComponent.messages[0];
			LOG::LogEntry& altLogCall = logComponent.messages[1];
			
			// Verify message content is the same
			Assert::AreEqual(baseLogCall.content, altLogCall.content);
			// Verify severity level is the same
			Assert::AreEqual((int)baseLogCall.severity, (int)altLogCall.severity);
		}

		TEST_METHOD(CHECK_LOGGER_WARNING)
		{
			Assert::AreEqual(0, 0);
		}

		TEST_METHOD(CHECK_LOGGER_ERROR)
		{
			Assert::AreEqual(0, 0);
		}

#pragma endregion

#pragma region Hierarchy Unit Tests

		TEST_METHOD(CHECK_HIERARCHY_TRANSLATE_PARENT)
		{
			Assert::AreEqual(0, 0);
		}

		TEST_METHOD(CHECK_HIERARCHY_ROTATE_PARENT)
		{
			Assert::AreEqual(0, 0);
		}

		TEST_METHOD(CHECK_HIERARCHY_SCALE_PARENT)
		{
			Assert::AreEqual(0, 0);
		}

#pragma endregion


	};
}
