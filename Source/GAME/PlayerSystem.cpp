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
		GW::MATH::GMATRIXF& _toManipulate = registry.get<Transform>(entity).transform;
		GW::MATH::GMatrix::TranslateLocalF(_toManipulate, deltaPos, _toManipulate);
	}

	GW::MATH::GVECTORF CalculateProjectileVelocity(entt::registry& registry, GW::MATH::GVECTORF inputs, std::string projectileNameIni)
	{
		float speed = registry.ctx().get<UTIL::Config>().gameConfig.get()->at(projectileNameIni).at("speed").as<float>();

		// Set Direction
		GW::MATH::GVECTORF result{ inputs.y - inputs.x, 0.0f, inputs.z - inputs.w, 0.0f };

		// determine if opposite only directions are firing
		if (inputs.y && inputs.x && result.z == 0)
		{
			result.x = 1;
		}
		else if (result.x == 0 && inputs.z && inputs.w)
		{
			result.z = 1;
		}

		// User is pressing all inputs, default it to shoot north east
		if (result.x == 0 && result.z == 0)
		{
			result.x = 1;
			result.z = 1;
		}

		// Normalize
		GW::MATH::GVector::NormalizeF(result, result);
		// Scale by speed from ini
		GW::MATH::GVector::ScaleF(result, speed, result);

		// UTIL::PrintVector(result);

		return result;
	}

	void CheckFiringState(entt::registry& registry, entt::entity entity)
	{
		// All player systems in here
		auto& input = registry.ctx().get<UTIL::Input>();
		auto& deltaTime = registry.ctx().get<UTIL::DeltaTime>();
		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

		if (!registry.any_of<FiringState>(entity))
		{
			// Firing State not on enemy, meaning we can fire!
			GW::MATH::GVECTORF inputKeys;
			// G_KEY_LEFT, G_KEY_RIGHT, G_KEY_DOWN, and G_KEY_UP
			input.immediateInput.GetState(G_KEY_LEFT, inputKeys.x);
			input.immediateInput.GetState(G_KEY_RIGHT, inputKeys.y);
			input.immediateInput.GetState(G_KEY_UP, inputKeys.z);
			input.immediateInput.GetState(G_KEY_DOWN, inputKeys.w);

			// just if any of these values are positive
			if (inputKeys.x || inputKeys.y || inputKeys.z || inputKeys.w)
			{
				auto bulletEntity = registry.create();
				auto& projectileVelocity = CalculateProjectileVelocity(registry, inputKeys, UTIL::Projectile1Name);

				registry.emplace<Bullet>(bulletEntity);
				UTIL::CreateModelEntity(registry, bulletEntity, UTIL::Projectile1Name, &registry.get<GAME::Transform>(entity).transform);
				registry.emplace<FiringState>(entity, config->at(UTIL::PlayerName).at("firerate").as<double>());
				registry.emplace<Velocity>(bulletEntity, projectileVelocity);
				registry.emplace<Collidable>(bulletEntity);
				registry.get<DRAW::MeshCollection>(bulletEntity).collider = registry.ctx().get<DRAW::ModelManager>().models[UTIL::Projectile1Name].collider;
			}
		}
		else
		{
			// Firing state on player, we should decrement cooldown!
			double& cd = registry.get<FiringState>(entity).cooldown;
			cd -= deltaTime.dtSec;
			if (cd <= 0)
			{
				registry.remove<FiringState>(entity);
			}
		}
	}

	void CheckInvulnerableState(entt::registry& registry, entt::entity entity)
	{
		auto invulnerablityComp = registry.try_get<Invulnerable>(entity);

		if (invulnerablityComp)
		{
			auto& deltaTime = registry.ctx().get<UTIL::DeltaTime>();
			invulnerablityComp->cooldown -= deltaTime.dtSec;

			if (invulnerablityComp->cooldown <= 0)
			{
				registry.remove<Invulnerable>(entity);
			}
		}
	}

	void UpdatePlayer(entt::registry& registry, entt::entity entity)
	{
		UpdateMovement(registry, entity);
		CheckFiringState(registry, entity);
		CheckInvulnerableState(registry, entity);
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<Player>().connect<UpdatePlayer>();
	}
} // namespace DRAW