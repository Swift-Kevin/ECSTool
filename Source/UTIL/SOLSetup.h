#ifndef SOL_H_
#define SOL_H_

#include "GameConfig.h"
#include "../DRAW/DrawComponents.h"
#include "../GAME/GameComponents.h"

namespace SOL
{
	entt::entity CreateMoon(entt::registry& registry, entt::entity parent, float radius);

	void SetupMercury(entt::registry& registry);
	void SetupVenus(entt::registry& registry);
	void SetupEarth(entt::registry& registry);
	void SetupMars(entt::registry& registry);
	void SetupJupiter(entt::registry& registry);
	void SetupSaturn(entt::registry& registry);
	void SetupUranus(entt::registry& registry);
	void SetupNeptune(entt::registry& registry);

} // namespace SOL
#endif // !SOL_H_