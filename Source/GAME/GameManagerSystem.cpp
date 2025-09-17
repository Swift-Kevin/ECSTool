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
			GAME::Transform& currTransform = registry.get<Transform>(entity);
			auto& currentMeshs = registry.get<DRAW::MeshCollection>(entity).entites;

			for (auto& mesh : currentMeshs)
			{
				// copy over transform to gpu instance
				registry.get<DRAW::GPUInstance>(mesh).transform = currTransform.world;
			}
		}
	}

	void UpdateWorldTransforms(entt::registry& registry)
	{
		auto view = registry.view<Transform>();

		for (auto entity : view)
		{
			auto& tf = registry.get<Transform>(entity);

			if (registry.all_of<ParentTransform>(entity))
			{
				auto& parentComp = registry.get<ParentTransform>(entity);
				if (parentComp.parent != entt::null && registry.all_of<Transform>(parentComp.parent))
				{
					auto& parentTf = registry.get<Transform>(parentComp.parent);
					GW::MATH::GMatrix::MultiplyMatrixF(tf.local, parentTf.world, tf.world);
				}
				else
				{
					tf.world = tf.local;
				}
			}
			else
			{
				tf.world = tf.local;
			}
		}
	}

	void UpdateOrbits(entt::registry& registry)
	{
		auto orbiters = registry.view<Transform, Orbit>();
		auto& deltaTime = registry.ctx().get<UTIL::DeltaTime>().dtSec;

		for (auto entity : orbiters)
		{
			auto& orbit = registry.get<Orbit>(entity);

			if (orbit.parent == entt::null || !registry.all_of<Transform>(orbit.parent))
			{
				continue;
			}

			auto& parentTransform = registry.get<Transform>(orbit.parent);
			auto& transform = registry.get<Transform>(entity);
			orbit.currentAngle += orbit.angularSpeed * deltaTime;

			GW::MATH::GMATRIXF rotation = GW::MATH::GIdentityMatrixF;
			switch (orbit.axis)
			{
			case GAME::ORBIT_AXIS::X:
			{
				GW::MATH::GMatrix::RotateXLocalF(rotation, orbit.currentAngle, rotation);
				break;
			}
			case GAME::ORBIT_AXIS::Y:
			{
				GW::MATH::GMatrix::RotateYLocalF(rotation, orbit.currentAngle, rotation);
				break;
			}
			case GAME::ORBIT_AXIS::Z:
			{
				GW::MATH::GMatrix::RotateZLocalF(rotation, orbit.currentAngle, rotation);
				break;
			}
			default:
				break;
			}

			GW::MATH::GVECTORF offset = { orbit.radius, 0, 0, 1 };
			GW::MATH::GMatrix::VectorXMatrixF(rotation, offset, offset);
			
			transform.local.row4 = offset; // local offset from parent/center
			transform.local.row4 = offset;
			transform.world = transform.local;
			GW::MATH::GMatrix::MultiplyMatrixF(transform.local, parentTransform.world, transform.world);
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