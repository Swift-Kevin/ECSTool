#include "../CCL.h"
#include "EngineComponents.h"
#include "../UTIL/Behaviors.h"

namespace CORE
{
	void ConnectEngine(entt::registry& registry, entt::entity entity)
	{

	}

	void UpdateEngineCoreLoop(entt::registry& registry, entt::entity entity)
	{
		BEHAVIORS::MainLoopBehavior(registry);
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_construct<Engine>().connect<ConnectEngine>();
		registry.on_update<Engine>().connect<UpdateEngineCoreLoop>();
	}
} // namespace DRAW