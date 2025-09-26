#include "pch.h"
#include "CppUnitTest.h"
#include "../Source/CCL.h"
#include "../Source/APP/ConsoleLog.h"
#include "../Source/GAME/GameComponents.h"
#include "../Source/UTIL/Utilities.h"
#include "../Source/UTIL/Behaviors.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ECSUnitTests
{
	TEST_CLASS(ECSUnitTests)
	{
	public:

#pragma region Logger Unit Tests

		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_LOGGER_DEBUG)
			TEST_OWNER(L"Logging Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()

			TEST_METHOD(CHECK_LOGGER_DEBUG)
		{
			/*
				Testing: Can users use both Log and Log Debug
				Verify: Severity (Debug) is equivalent.
			*/

			// initialize info
			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<LOG::Logs>();
				LOG::InitializeLogSystem(registry);
			}

			// Test Info
			std::string logMsg = "Example Log Message - TEST LOG DEBUG LEVEL";
			LOG::Log(logMsg.c_str());
			LOG::LogDebug(logMsg.c_str());

			// Validate Data
			{
				LOG::Logs& logComponent = registry.ctx().get<LOG::Logs>();
				LOG::LogEntry& baseLogCall = logComponent.messages[0];
				LOG::LogEntry& altLogCall = logComponent.messages[1];

				Assert::AreEqual((int)baseLogCall.severity, (int)altLogCall.severity);
			}
		}


		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_LOGGER_WARNING)
			TEST_OWNER(L"Logging Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_LOGGER_WARNING)
		{
			/*
				Testing: Can users use both Log and Log Warning
				Verify: Severity (Warning) is equivalent.
			*/

			// initialize info
			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<LOG::Logs>();
				LOG::InitializeLogSystem(registry);
			}

			// Test Info
			std::string logMsg = "Example Log Message - TEST LOG WARNING LEVEL";
			LOG::Log(logMsg.c_str(), LOG::LogSeverity::Warning);
			LOG::LogWarning(logMsg.c_str());

			// Validate Data
			{
				LOG::Logs& logComponent = registry.ctx().get<LOG::Logs>();
				LOG::LogEntry& baseLogCall = logComponent.messages[0];
				LOG::LogEntry& altLogCall = logComponent.messages[1];

				Assert::AreEqual((int)baseLogCall.severity, (int)altLogCall.severity);
			}
		}


		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_LOGGER_ERROR)
			TEST_OWNER(L"Logging Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_LOGGER_ERROR)
		{
			/*
				Testing: Can users use both Log and Log Error
				Verify: Severity (Error) is equivalent.
			*/

			// initialize info
			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<LOG::Logs>();
				LOG::InitializeLogSystem(registry);
			}

			// Test Info
			std::string logMsg = "Example Log Message - TEST LOG ERROR LEVEL";
			LOG::Log(logMsg.c_str(), LOG::LogSeverity::Error);
			LOG::LogError(logMsg.c_str());

			// Validate Data
			{
				LOG::Logs& logComponent = registry.ctx().get<LOG::Logs>();
				LOG::LogEntry& baseLogCall = logComponent.messages[0];
				LOG::LogEntry& altLogCall = logComponent.messages[1];

				Assert::AreEqual((int)baseLogCall.severity, (int)altLogCall.severity);
			}
		}


		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_LOGGER_CONTENT)
			TEST_OWNER(L"Logging Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_LOGGER_CONTENT)
		{
			/*
				Testing: Content value within the registry
				Verify: message content is exactly the same
			*/

			// initialize info
			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<LOG::Logs>();
				LOG::InitializeLogSystem(registry);
			}

			// Test Info
			std::string logMsg = "Example Log Message... this log should be duplicated, well... hopefully";
			LOG::Log(logMsg.c_str());
			LOG::LogDebug(logMsg.c_str());
			LOG::LogWarning(logMsg.c_str());
			LOG::LogError(logMsg.c_str());

			// Validate Data
			{
				LOG::Logs& logComponent = registry.ctx().get<LOG::Logs>();
				LOG::LogEntry& logA = logComponent.messages[0];
				LOG::LogEntry& logB = logComponent.messages[1];
				LOG::LogEntry& logC = logComponent.messages[2];
				LOG::LogEntry& logD = logComponent.messages[3];

				Assert::AreEqual(logA.content, logMsg);
				Assert::AreEqual(logB.content, logMsg);
				Assert::AreEqual(logC.content, logMsg);
				Assert::AreEqual(logD.content, logMsg);
			}
		}


		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_LOGGER_ORDER)
			TEST_OWNER(L"Logging Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_LOGGER_ORDER)
		{
			/*
				Testing: Logs are sent and stored in correct order
				Verify: order of logs
			*/

			// initialize info
			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<LOG::Logs>();
				LOG::InitializeLogSystem(registry);
			}

			// test info
			LOG::Log("First Log");
			LOG::Log("Second Log");
			LOG::Log("Third Log");

			// validate Data
			LOG::Logs& logComponent = registry.ctx().get<LOG::Logs>();
			LOG::LogEntry first = logComponent.messages[0];
			LOG::LogEntry second = logComponent.messages[1];
			LOG::LogEntry third = logComponent.messages[2];

			Assert::AreEqual("First Log", first.content.c_str());
			Assert::AreEqual("Second Log", second.content.c_str());
			Assert::AreEqual("Third Log", third.content.c_str());
		}

#pragma endregion

#pragma region Hierarchy Unit Tests

		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_HIERARCHY_TRANSLATE)
			TEST_OWNER(L"Hierarchy Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_HIERARCHY_TRANSLATE)
		{
			/*
				Testing: Parent Translation affects child translation
				Verify: Child position reflects change properly.
			*/

			// initialize info
			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<UTIL::Config>();
			}

			// create transforms and compute childs local
			entt::entity parent = registry.create();
			entt::entity child = registry.create();
			auto& parentTransform = registry.emplace<GAME::Transform>(parent);
			auto& childTransform = registry.emplace<GAME::Transform>(child);
			registry.emplace<GAME::ChildTransform>(child, parent);

			// setup transform data
			{
				parentTransform.world = UTIL::GetRandomTransform({ -10, -10, -10 }, { 10, 10, 10 });
				childTransform.world = UTIL::GetRandomTransform({ -10, -10, -10 }, { 10, 10, 10 });
				UTIL::ComputeChildLocal(parentTransform.world, childTransform);
			}

			// create test data
			GW::MATH::GVECTORF translationVector = UTIL::GetRandomTransform({ 10, 10, 10 }, { 20, 20, 20 }).row4;
			GW::MATH::GMatrix::TranslateGlobalF(parentTransform.world, translationVector, parentTransform.world);
			// collect states
			GW::MATH::GVECTORF beforeChildWorldPos = childTransform.world.row4;
			UTIL::ComputeHierarchy(registry);
			GW::MATH::GVECTORF afterChildWorldPos = childTransform.world.row4;

			// validate data
			GW::MATH::GVECTORF differential = { 0, 0, 0 };
			GW::MATH::GVector::SubtractVectorF(afterChildWorldPos, beforeChildWorldPos, differential);

			// verify each axis
			Assert::AreEqual(differential.x, translationVector.x, 0.005f);
			Assert::AreEqual(differential.y, translationVector.y, 0.005f);
			Assert::AreEqual(differential.z, translationVector.z, 0.005f);
		}

		
		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_HIERARCHY_ROTATE_PARENT)
			TEST_OWNER(L"Hierarchy Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_HIERARCHY_ROTATE_PARENT)
		{
			/*
				Testing: Parent Rotation affects childs position
				Verify: Child position changed from 1,0,0 to 0,1,0 .
			*/

			// initialize info
			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<UTIL::Config>();
			}

			// create transforms and compute childs local
			entt::entity parent = registry.create();
			entt::entity child = registry.create();
			auto& parentTransform = registry.emplace<GAME::Transform>(parent);
			auto& childTransform = registry.emplace<GAME::Transform>(child);
			registry.emplace<GAME::ChildTransform>(child, parent);

			// setup transform data
			{
				parentTransform.world = GW::MATH::GIdentityMatrixF;
				childTransform.world = GW::MATH::GIdentityMatrixF;
				childTransform.world.row4 = { 1, 0, 0 };
				UTIL::ComputeChildLocal(parentTransform.world, childTransform);
			}

			// create test data
			GW::MATH::GMatrix::RotateZGlobalF(parentTransform.world, G_DEGREE_TO_RADIAN_F(90.0f), parentTransform.world);
			UTIL::ComputeHierarchy(registry);

			// verify each axis
			Assert::AreEqual(childTransform.world.row4.x, 0, 0.005f);
			Assert::AreEqual(childTransform.world.row4.y, 1, 0.005f);
			Assert::AreEqual(childTransform.world.row4.z, 0, 0.005f);
		}

		
		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_HIERARCHY_SCALE_PARENT)
			TEST_OWNER(L"Hierarchy Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_HIERARCHY_SCALE_PARENT)
		{
			/*
				Testing: Parent Rotation affects childs position
				Verify: Child position changed from 1,0,0 to 0,1,0 .
			*/

			// initialize info
			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<UTIL::Config>();
			}

			// create transforms and compute childs local
			entt::entity parent = registry.create();
			entt::entity child = registry.create();
			auto& parentTransform = registry.emplace<GAME::Transform>(parent);
			auto& childTransform = registry.emplace<GAME::Transform>(child);
			registry.emplace<GAME::ChildTransform>(child, parent);

			// setup transform data
			{
				parentTransform.world = UTIL::GetRandomTransform({ -10, -10, -10 }, { 10, 10, 10 });
				childTransform.world = UTIL::GetRandomTransform({ -10, -10, -10 }, { 10, 10, 10 });
				GW::MATH::GMatrix::ScaleGlobalF(childTransform.world, GW::MATH::GVECTORF{ 0.25f, 0.25f, 0.25f }, childTransform.world);
				UTIL::ComputeChildLocal(parentTransform.world, childTransform);
			}

			// create test data
			GW::MATH::GMatrix::ScaleGlobalF(parentTransform.world, GW::MATH::GVECTORF{ 4, 4, 4 }, parentTransform.world);
			UTIL::ComputeHierarchy(registry);
			GW::MATH::GVECTORF childScaled = { 0, 0, 0, 1 };
			GW::MATH::GMatrix::GetScaleF(childTransform.world, childScaled);

			// verify each axis
			Assert::AreEqual(childScaled.x, 1, 0.005f);
			Assert::AreEqual(childScaled.y, 1, 0.005f);
			Assert::AreEqual(childScaled.z, 1, 0.005f);
		}

		
		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_HIERARCHY_MULTI_LEVEL)
			TEST_OWNER(L"Hierarchy Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_HIERARCHY_MULTI_LEVEL)
		{
			/*
				Testing: Hiearchy levels
				Verify: Transform works across multiple levels
			*/

			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<UTIL::Config>();
			}

			// create entities
			entt::entity parent = registry.create();
			entt::entity child = registry.create();
			entt::entity grandchild = registry.create();

			auto& parentTransform = registry.emplace<GAME::Transform>(parent);
			auto& childTransform = registry.emplace<GAME::Transform>(child);
			auto& grandTransform = registry.emplace<GAME::Transform>(grandchild);

			// setup test info
			{
				registry.emplace<GAME::ChildTransform>(child, parent);
				registry.emplace<GAME::ChildTransform>(grandchild, child);

				childTransform.world.row4 = { 1,0,0 };
				grandTransform.world.row4 = { 0,1,0 };
				UTIL::ComputeChildLocal(parentTransform.world, childTransform);
				UTIL::ComputeChildLocal(childTransform.world, grandTransform);
			}

			// test values
			GW::MATH::GMatrix::RotateZGlobalF(parentTransform.world, G_DEGREE_TO_RADIAN_F(90.0f), parentTransform.world);
			UTIL::ComputeHierarchy(registry);

			// expected values
			GW::MATH::GMATRIXF expectedChild;
			GW::MATH::GMatrix::MultiplyMatrixF(childTransform.local, parentTransform.world, expectedChild);
			GW::MATH::GMATRIXF expectedGrand;
			GW::MATH::GMatrix::MultiplyMatrixF(grandTransform.local, expectedChild, expectedGrand);

			// validate
			Assert::AreEqual(grandTransform.world.row4.x, expectedGrand.row4.x, 0.005f);
			Assert::AreEqual(grandTransform.world.row4.y, expectedGrand.row4.y, 0.005f);
			Assert::AreEqual(grandTransform.world.row4.z, expectedGrand.row4.z, 0.005f);
		}

#pragma endregion

#pragma region Utility Methods Unit Tests

		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_CREATE_MOON)
			TEST_OWNER(L"Utility Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_CREATE_MOON)
		{
			/*
				Testing: Moon creation relative to the sun within a range.
				Verify: Has Moon Component, Childed to Sun.
			*/

			// initialize info
			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<UTIL::Config>();
			}

			// setup test data
			entt::entity sun = registry.create();
			entt::entity moon = SOL::CreateMoon(registry, sun, 5);

			// validate data
			entt::entity grabbedMoon = registry.group<GAME::Moon>().front();

			bool wasMoonMade = grabbedMoon != entt::null && grabbedMoon == moon;
			bool isChildOfSun = registry.get<GAME::ChildTransform>(grabbedMoon).parent == sun;

			Assert::IsTrue(wasMoonMade);
			Assert::IsTrue(isChildOfSun);
		}

		
		BEGIN_TEST_METHOD_ATTRIBUTE(CHECK_UTIL_RANDOM_TRANSFORM_BOUNDS)
			TEST_OWNER(L"Utility Unit Tests")
			END_TEST_METHOD_ATTRIBUTE()
			TEST_METHOD(CHECK_UTIL_RANDOM_TRANSFORM_BOUNDS)
		{
			/*
				Testing: Random Utility Transform makes position within bounds
				Verify: row4 is within bounds set
			*/

			entt::registry registry;
			{
				CCL::InitializeComponentLogic(registry);
				registry.ctx().emplace<UTIL::Config>();
			}

			GW::MATH::GVECTORF min = { -50, -50, -50, 1 };
			GW::MATH::GVECTORF max = { 50,  50,  50, 1 };

			for (int i = 0; i < 50; ++i)
			{
				GW::MATH::GMATRIXF t = UTIL::GetRandomTransform(min, max);
				auto pos = t.row4;

				Assert::IsTrue(pos.x >= min.x && pos.x <= max.x);
				Assert::IsTrue(pos.y >= min.y && pos.y <= max.y);
				Assert::IsTrue(pos.z >= min.z && pos.z <= max.z);
			}
		}

#pragma endregion


	};
}
