#ifndef GAME_COMPONENTS_H_
#define GAME_COMPONENTS_H_

namespace GAME
{
	///*** Tags ***///
	struct Player {};
	struct Obstacle {};
	struct Collidable {};
	struct GameManager {};

	///*** Components ***///
	struct Transform {
		GW::MATH::GMATRIXF transform;

		GW::MATH::GVECTORF& Position() { return transform.row4; };
	};

	struct Velocity {
		GW::MATH::GVECTORF velocity;
	};

}// namespace GAME
#endif // !GAME_COMPONENTS_H_