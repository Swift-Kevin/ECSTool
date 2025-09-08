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

		GW::MATH::GVECTORF& Position() { return transform.row4; };
	};

	struct GameManager {

	};

	struct FiringState {
		double cooldown = 0;
	};

	struct Velocity {
		GW::MATH::GVECTORF velocity;
	};

	struct Health {
		int hitpoints = 0;
	};

	struct Shatters {
		int initialShatterCount = 0;
		int shatterAmount = 0;
		float shatterScale = 0.0f;
	};

	struct Invulnerable {
		double cooldown = 0;
	};

}// namespace GAME
#endif // !GAME_COMPONENTS_H_