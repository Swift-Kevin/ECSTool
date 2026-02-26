#ifndef BEHAVIORS_H_
#define BEHAVIORS_H_

#include "../CCL.h"
#include "../UTIL/Utilities.h"
#include "../APP/Window.hpp"
#include "../DRAW/DrawComponents.h"
#include "../GAME/GameComponents.h"
#include "../UI/UserInterfaceComponents.h"
#include "../UTIL/SOLSetup.h"

namespace BEHAVIORS
{
	// Method declarations
	void InitializeGraphics(entt::registry& registry);
	void InitializeUI(entt::registry& registry);
	void InitializeGameplay(entt::registry& registry);
	void MainLoopBehavior(entt::registry& registry);

} // namespace BEHAVIORS
#endif // !BEHAVIORS_H_
