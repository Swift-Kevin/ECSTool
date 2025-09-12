#ifndef GAME_COMPONENTS_H_
#define GAME_COMPONENTS_H_

#include "../UTIL/ComponentReflection.h"

namespace GAME
{
	///*** Tags ***///
	COMPONENT(Player) {};
	COMPONENT(Obstacle) {};
	COMPONENT(Collidable) {};
	COMPONENT(GameManager) {};

	///*** Components ***///
	COMPONENT(Transform)
	{
		GW::MATH::GMATRIXF transform;
		GW::MATH::GVECTORF& Position() { return transform.row4; };
	};

	COMPONENT(Velocity) 
	{
		GW::MATH::GVECTORF velocity;
	};

}// namespace GAME
#endif // !GAME_COMPONENTS_H_