#ifndef GAME_COMPONENTS_H_
#define GAME_COMPONENTS_H_

#include "../UTIL/ComponentReflection.h"

namespace GAME
{
	enum class ORBIT_AXIS : byte
	{
		X,
		Y,
		Z,
	};

	///*** Tags ***///
	COMPONENT(Sun) {};
	COMPONENT(Earth) {};
	COMPONENT(Player) {};
	COMPONENT(Obstacle) {};
	COMPONENT(Collidable) {};
	COMPONENT(GameManager) {};
	COMPONENT(StressTestAddition) {};

	///*** Components ***///
	COMPONENT(Transform)
	{
		GW::MATH::GMATRIXF local = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMATRIXF world = GW::MATH::GIdentityMatrixF;
	};

	COMPONENT(ChildTransform)
	{
		entt::entity parent = entt::null;
	};

	COMPONENT(Orbit)
	{
		entt::entity parent = entt::null;
		ORBIT_AXIS axis = ORBIT_AXIS::Y;
		float angularSpeed = 1.0f;
		float currentAngle = 0.0f;
		float currentOffset = 0.0f;
	};

	COMPONENT(Velocity)
	{
		GW::MATH::GVECTORF velocity;
	};

}// namespace GAME
#endif // !GAME_COMPONENTS_H_