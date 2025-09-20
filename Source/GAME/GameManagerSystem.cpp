#include "GAMEComponents.h"
#include "../CCL.h"

#include "../DRAW/DrawComponents.h"
#include "../UTIL/Utilities.h"

namespace GAME
{
	void UpdateMeshTransforms(entt::registry& registry)
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
		auto transforms = registry.view<Transform, Orbit, ParentTransform>();
		auto deltaTime = registry.ctx().get<UTIL::DeltaTime>();
		bool observeChange = false;

		for (auto [entity, transform, orbit, parentEntity] : transforms.each())
		{
			observeChange = registry.any_of<GAME::Observe>(entity);

			auto& child = registry.get<Transform>(entity);
			auto parent = registry.get<Transform>(parentEntity.parent);

			UTIL::DebugInfo dInfo = registry.ctx().get<UTIL::DebugInfo>();
			GW::MATH::GVECTORF translate = { 0, dInfo.theta, 0, 1 };
			GW::MATH::GVECTORF scale = { dInfo.theta, dInfo.theta, dInfo.theta, 1 };

			switch (dInfo.debugMode)
			{
			case UTIL::DebugHierarchy::Rotation:
			{
				GW::MATH::GMatrix::RotateYGlobalF(parent.world, orbit.currentAngle, parent.world);
				break;
			}
			case UTIL::DebugHierarchy::Translation:
			{
				if (observeChange)
					GW::MATH::GMatrix::TranslateGlobalF(parent.world, translate, parent.world);
				break;
			}
			case UTIL::DebugHierarchy::Scale:
			{
				if (observeChange)
					GW::MATH::GMatrix::ScaleLocalF(parent.world, scale, parent.world);
				break;
			}
			case UTIL::DebugHierarchy::Combined:
			{
				if (observeChange)
					GW::MATH::GMatrix::TranslateGlobalF(parent.world, translate, parent.world);

				GW::MATH::GMatrix::RotateYGlobalF(parent.world, orbit.currentAngle, parent.world);

				if (observeChange)
					GW::MATH::GMatrix::ScaleLocalF(parent.world, scale, parent.world);

				break;
			}
			case UTIL::DebugHierarchy::SolarSystem:
			{
				GW::MATH::GMatrix::RotateYGlobalF(parent.world, orbit.currentAngle, parent.world);
				if (observeChange)
					GW::MATH::GMatrix::TranslateGlobalF(parent.world, translate, parent.world);
				break;
			}
			default:
				break;
			}

			GW::MATH::GMatrix::MultiplyMatrixF(child.local, parent.world, child.world);
		}
	}

	void UpdateOrbits(entt::registry& registry)
	{
		auto orbiters = registry.view<Transform, Orbit>();
		auto& deltaTime = registry.ctx().get<UTIL::DeltaTime>().dtSec;

		for (auto entity : orbiters)
		{
			auto& orbit = registry.get<Orbit>(entity);
			orbit.currentAngle += orbit.angularSpeed * deltaTime;
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
		UpdateMeshTransforms(registry);
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<GameManager>().connect<UpdateGameManager>();
	}
} // namespace DRAW