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
	// Helper
	int AddEntites(int numberToAdd = 0);
	bool CheckAddEntites();

	// Method declarations
	void GraphicsBehavior(entt::registry& registry);
	void UIBehavior(entt::registry& registry);
	void GameplayBehavior(entt::registry& registry);
	void MainLoopBehavior(entt::registry& registry);

} // namespace BEHAVIORS
#endif // !BEHAVIORS_H_
