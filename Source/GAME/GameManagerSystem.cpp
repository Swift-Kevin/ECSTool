#include "GAMEComponents.h"
#include "../CCL.h"

#include "../DRAW/DrawComponents.h"
#include "../UTIL/Utilities.h"

namespace GAME
{
	void UpdateMeshTransforms(entt::registry& registry)
	{
		auto& allEntities = registry.view<Transform, DRAW::MeshCollection>();

		for (const entt::entity& entity : allEntities)
		{
			// can use view for accessing the transform
			GW::MATH::GMATRIXF& currTransform = registry.get<Transform>(entity).transform;
			auto& currentMeshs = registry.get<DRAW::MeshCollection>(entity).entites;

			for (auto& mesh : currentMeshs)
			{
				// copy over transform to gpu instance
				registry.get<DRAW::GPUInstance>(mesh).transform = currTransform;
			}
		}
	}

	void UpdateEntityVelocities(entt::registry& registry)
	{
		// Only delta time is important in this
		auto& deltaTime = registry.ctx().get<UTIL::DeltaTime>().dtSec;
		auto& allEntities = registry.view<Transform, Velocity, DRAW::MeshCollection>();

		// Update only the entities and not the meshs
		for (const entt::entity& entity : allEntities)
		{
			// add velocity to position
			auto& pos = registry.get<Transform>(entity).transform.row4;
			auto velocity = registry.get<Velocity>(entity).velocity;

			GW::MATH::GVector::ScaleF(velocity, deltaTime, velocity);
			GW::MATH::GVector::AddVectorF(pos, velocity, pos);
		}
	}

	void UpdateGameManager(entt::registry& registry, entt::entity entity)
	{
		if (registry.any_of<GameOver>(entity))
			return;

		// Update Velocities and Transforms
		UpdateEntityVelocities(registry);
		UpdateMeshTransforms(registry);

		auto allPlayers = registry.group<Player>();
		for (auto currentPlayer : allPlayers)
		{
			registry.patch<Player>(currentPlayer);
		}
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<GameManager>().connect<UpdateGameManager>();
	}
} // namespace DRAW