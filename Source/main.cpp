
#include "../Source/UTIL/Behaviors.h"

int main()
{
	// All components, tags, and systems are stored in a single registry
	entt::registry registry;

	// initialize the ECS Component Logic
	CCL::InitializeComponentLogic(registry);

	// Seed the rand
	unsigned int time = std::chrono::steady_clock::now().time_since_epoch().count();
	srand(time);

	registry.ctx().emplace<UTIL::Config>();

	BEHAVIORS::GraphicsBehavior(registry); // create windows, surfaces, and renderers

	BEHAVIORS::GameplayBehavior(registry); // create entities and components for gameplay

	BEHAVIORS::MainLoopBehavior(registry); // update windows and input

	// clear all entities and components from the registry
	// invokes on_destroy() for all components that have it
	// registry will still be intact while this is happening
	registry.clear();

	return 0; // now destructors will be called for all components
}
