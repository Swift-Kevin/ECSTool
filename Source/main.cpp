#include "../Source/UTIL/Behaviors.h"

int main()
{
	entt::registry registry;
	CCL::InitializeComponentLogic(registry);

	// Seed the rand
	unsigned int time = std::chrono::steady_clock::now().time_since_epoch().count();
	srand(time);

	registry.ctx().emplace<UTIL::Config>();

	BEHAVIORS::GraphicsBehavior(registry); 
	BEHAVIORS::UIBehavior(registry);
	BEHAVIORS::GameplayBehavior(registry); 
	BEHAVIORS::MainLoopBehavior(registry); 

	// Calls all on_destroys and clears the ECS
	registry.clear();

	return 0;
}
