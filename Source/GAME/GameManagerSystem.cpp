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
				registry.get<DRAW::GPUInstance>(entity).transform = currTransform.worldMatrix;
			}
		}
	}

	void UpdateTransforms(entt::registry& registry)
	{
		auto view = registry.view<Transform>();

		auto& deltaTime = registry.ctx().get<UTIL::DeltaTime>();
		auto& debugInfo = registry.ctx().get<UTIL::DebugInfo>();

		for (auto& [entity, child] : view.each())
		{
			auto* parentTransformComp = registry.try_get<GAME::Transform>(child.parentID);
			auto* orbitComp = registry.try_get<GAME::Orbit>(entity);

			switch (debugInfo.debugMode)
			{
			case UTIL::DebugHierarchy::SolarSystem:
			{
				float rate = deltaTime.dtSec;
				if (orbitComp) { rate *= orbitComp->angularSpeed; }
				else { rate = 0.0f; }

				if (parentTransformComp)
				{
					parentTransformComp->localRotation.y += rate;
				}
				else
				{
					child.localRotation.y += rate;
				}
				break;
			}

			default:
				break;
			}

			// Rebuild parent matrices from TRS
			if (parentTransformComp)
			{
				parentTransformComp->GetLocalMatrix();
				GW::MATH::GMatrix::MultiplyMatrixF(child.GetLocalMatrix(), parentTransformComp->worldMatrix, child.worldMatrix);
			}
			else
			{
				child.worldMatrix = child.GetLocalMatrix();
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
			auto& pos = registry.get<Transform>(entity).localTranslation;
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
		UpdateTransforms(registry);
		UpdateMeshTransforms(registry);
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<GameManager>().connect<UpdateGameManager>();
	}
} // namespace DRAW