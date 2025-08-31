#include "GAMEComponents.h"
#include "../CCL.h"

#include "../UTIL/Utilities.h"

namespace GAME
{
	void UpdateMovement(entt::registry& registry, entt::entity entity)
	{
		auto& input = registry.ctx().get<UTIL::Input>();
		auto& deltaTime = registry.ctx().get<UTIL::DeltaTime>();
		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

		float speed = config->at(UTIL::PlayerName).at("speed").as<float>();
		float keyboardPositive = 0, keyboardNegative = 0;

		// total vertical (z b/c top-down) movement
		input.immediateInput.GetState(G_KEY_W, keyboardPositive);
		input.immediateInput.GetState(G_KEY_S, keyboardNegative);
		float totalZ = (keyboardPositive - keyboardNegative) * (speed * deltaTime.dtSec);

		input.immediateInput.GetState(G_KEY_D, keyboardPositive);
		input.immediateInput.GetState(G_KEY_A, keyboardNegative);
		float totalX = (keyboardPositive - keyboardNegative) * (speed * deltaTime.dtSec);

		// Translate Locally on X and Z
		GW::MATH::GVECTORF deltaPos = { totalX, 0, totalZ, 1 };
		Transform* _toManipulate = registry.try_get<Transform>(entity);
		if (_toManipulate)
		{
			GW::MATH::GMatrix::TranslateLocalF(_toManipulate->transform, deltaPos, _toManipulate->transform);
		}
	}

	void UpdatePlayer(entt::registry& registry, entt::entity entity)
	{
		UpdateMovement(registry, entity);
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<Player>().connect<UpdatePlayer>();
	}
} // namespace DRAW