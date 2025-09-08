#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "GameConfig.h"
#include "../DRAW/DrawComponents.h"
#include "../GAME/GameComponents.h"

namespace UTIL
{
	static const std::string Enemy1Name = "Enemy1";
	static const std::string PlayerName = "Player";
	static const std::string Projectile1Name = "Bullet";

	struct Config
	{
		std::shared_ptr<GameConfig> gameConfig = std::make_shared<GameConfig>();
	};

	struct DeltaTime
	{
		double dtSec;
	};

	struct Input
	{
		GW::INPUT::GController gamePads; // controller support
		GW::INPUT::GInput immediateInput; // twitch keybaord/mouse
		GW::INPUT::GBufferedInput bufferedInput; // event keyboard/mouse
	};

	/// Method declarations
	void CreateModelEntity(entt::registry& registry, entt::entity entity, std::string entityName, GW::MATH::GMATRIXF* _transform = nullptr);
	void SetupCamera(entt::registry& registry, entt::entity entity);

	/// Creates a normalized vector pointing in a random direction on the X/Z plane
	GW::MATH::GVECTORF GetRandomVelocityVector();
	GW::MATH::GMATRIXF GetRandomTransform(GW::MATH::GVECTORF min, GW::MATH::GVECTORF max);

	float GetRandomRange(float min = 0, float max = 0);

	void PrintVector(GW::MATH::GVECTORF toPrint);

} // namespace UTIL
#endif // !UTILITIES_H_