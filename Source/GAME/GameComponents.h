#ifndef GAME_COMPONENTS_H_
#define GAME_COMPONENTS_H_

namespace GAME
{
	///*** Tags ***///
	struct Player {};
	struct Enemy {};
	struct Bullet {};
	struct Obstacle {};
	struct Collidable {};
	struct ToDestroy {};
	struct GameOver {};

	///*** Components ***///
	struct Transform {
		GW::MATH::GMATRIXF transform;
	};

	struct GameManager {

	};

	struct Velocity {
		GW::MATH::GVECTORF velocity;
	};

}// namespace GAME
#endif // !GAME_COMPONENTS_H_