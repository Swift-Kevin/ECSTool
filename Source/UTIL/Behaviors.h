#ifndef BEHAVIORS_H_
#define BEHAVIORS_H_

#include "../CCL.h"
#include "../UTIL/Utilities.h"
#include "../APP/Window.hpp"
#include "../DRAW/DrawComponents.h"
#include "../GAME/GameComponents.h"

namespace BEHAVIORS
{
	// Method declarations
	void GraphicsBehavior(entt::registry& registry);
	void GameplayBehavior(entt::registry& registry);
	void MainLoopBehavior(entt::registry& registry);

} // namespace BEHAVIORS
#endif // !BEHAVIORS_H_
