#include "GAMEComponents.h"
#include "../CCL.h"

#include "../DRAW/DrawComponents.h"
#include "../UTIL/Utilities.h"
#include "Hierarchy.h"

namespace GAME
{
	void UpdateGPUTransforms(entt::registry& registry)
	{
		auto& drawables = registry.view<Transform, DRAW::MeshCollection>();

		for (const entt::entity& entity : drawables)
		{
			GAME::Transform& currTransform = registry.get<Transform>(entity);
			auto& entities = registry.get<DRAW::MeshCollection>(entity).entites;

			for (auto& entity : entities)
			{
				// copy over transform to gpu instance
				// gpu instance is what is actually drawn
				registry.get<DRAW::GPUInstance>(entity).transform = currTransform.world;
			}
		}
	}

	void UpdateWorldTransforms(entt::registry& registry)
	{
		auto& entities = registry.view<Transform, ParentTransform>();

		for (auto entity : entities)
		{
			auto& transform = registry.get<Transform>(entity);
			entt::entity parent = registry.get<Orbit>(entity).parent;
			auto& parentTransform = registry.get<Transform>(parent);

			GW::MATH::GMatrix::MultiplyMatrixF(transform.local, parentTransform.world, transform.world);
		}
	}

	void UpdateOrbits(entt::registry& registry)
	{
		auto& deltaTime = registry.ctx().get<UTIL::DeltaTime>().dtSec;

		for (auto [entity, orbit, transform] : registry.view<Orbit, Transform>().each())
		{
			orbit.currentAngle = orbit.angularSpeed * deltaTime;

			auto& parentTransform = registry.get<Transform>(orbit.parent);
			auto& childTransform = registry.get<Transform>(entity);

			// get local matrix?
			GW::MATH::GMATRIXF parentInv;
			GW::MATH::GMatrix::InverseF(parentTransform.world, parentInv);
			GW::MATH::GMatrix::MultiplyMatrixF(childTransform.world, parentInv, childTransform.local);

			// calculate local
			std::cout << "Local "; UTIL::PrintVector(childTransform.local.row4);
			std::cout << "Global "; UTIL::PrintVector(childTransform.world.row4);
			GW::MATH::GMatrix::RotateYGlobalF(childTransform.local, orbit.currentAngle, childTransform.local);
			GW::MATH::GMatrix::MultiplyMatrixF(parentTransform.world, childTransform.local, childTransform.world);
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
			auto& pos = registry.get<Transform>(entity).local.row4;
			auto velocity = registry.get<Velocity>(entity).velocity;

			GW::MATH::GVector::ScaleF(velocity, deltaTime, velocity);
			GW::MATH::GVector::AddVectorF(pos, velocity, pos);
		}
	}

	void UpdateGameManager(entt::registry& registry, entt::entity entity)
	{
		auto allPlayers = registry.group<Player>();
		for (auto currentPlayer : allPlayers)
		{
			registry.patch<Player>(currentPlayer);
		}

		// Update Velocities and Transforms
		UpdateEntityVelocities(registry);
		UpdateOrbits(registry);
		UpdateWorldTransforms(registry);
		UpdateGPUTransforms(registry);
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<GameManager>().connect<UpdateGameManager>();
	}
} // namespace DRAW