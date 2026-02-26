#include "../Source/CCL.h"
#include "../Source/CORE/EngineComponents.h"
#include "../Source/UTIL/Behaviors.h"

int main()
{
	entt::registry registry;
	CCL::InitializeComponentLogic(registry);

	unsigned int time = std::chrono::steady_clock::now().time_since_epoch().count();
	srand(time);
	
	registry.ctx().emplace<UTIL::Config>();
	registry.ctx().emplace<LOG::Logs>();
	LOG::InitializeLogSystem(registry);
	
	BEHAVIORS::InitializeGraphics(registry);
	BEHAVIORS::InitializeUI(registry);
	BEHAVIORS::InitializeGameplay(registry);

	registry.ctx().emplace<CORE::Engine>();
	BEHAVIORS::MainLoopBehavior(registry);
	registry.clear();

	return 0;
}
