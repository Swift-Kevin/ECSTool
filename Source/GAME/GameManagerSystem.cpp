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

	void BounceEnemy(GW::MATH::GVECTORF& _pos, GW::MATH::GVECTORF& _velocity, GW::MATH::GOBBF& _colliderHit)
	{
		// w = v – (2 * (v dot n) * n)
		// 'w' is the new velocity,
		// 'v' is the current velocity,
		// 'n' is the normal of the surface

		// Get Point to Surface
		GW::MATH::GVECTORF closestPoint;
		GW::MATH::GCollision::ClosestPointToOBBF(_colliderHit, _pos, closestPoint);

		// Get Normal
		GW::MATH::GVECTORF normal;
		GW::MATH::GVector::SubtractVectorF(_pos, closestPoint, normal);
		normal.y = 0;
		normal.w = 0;
		GW::MATH::GVector::NormalizeF(normal, normal);

		float dotVal;
		GW::MATH::GVector::DotF(_velocity, normal, dotVal);
		dotVal *= 2;
		GW::MATH::GVECTORF scaledNormal;
		GW::MATH::GVector::ScaleF(normal, dotVal, scaledNormal);
		GW::MATH::GVector::SubtractVectorF(_velocity, scaledNormal, _velocity);
	}

	void HandleCollisions(entt::registry& registry)
	{
		auto& collisons = registry.view<Transform, DRAW::MeshCollection, Collidable>();
		for (auto i = collisons.begin(); i != collisons.end(); i++)
		{
			using namespace GW::MATH;
			auto actingCollider = registry.get<DRAW::MeshCollection>(*i).collider;
			auto& actingTransform = registry.get<Transform>(*i).transform;

			// Move to world space
			GMatrix::VectorXMatrixF(actingTransform, actingCollider.center, actingCollider.center);

			// Scale the extents
			GVECTORF scaleA;
			GMatrix::GetScaleF(actingTransform, scaleA);
			actingCollider.extent.x *= scaleA.x;
			actingCollider.extent.y *= scaleA.y;
			actingCollider.extent.z *= scaleA.z;

			// Set rotation
			GQUATERNIONF quatActing;
			GQuaternion::SetByMatrixF(actingTransform, quatActing);
			GQuaternion::MultiplyQuaternionF(actingCollider.rotation, quatActing, actingCollider.rotation);

			auto j = i;
			for (j++; j != collisons.end(); j++)
			{
				auto testCollider = registry.get<DRAW::MeshCollection>(*j).collider;
				auto& testTransform = registry.get<Transform>(*j).transform;

				// Move to world space
				GMatrix::VectorXMatrixF(testTransform, testCollider.center, testCollider.center);

				// Scale the extents
				GVECTORF testScale;
				GMatrix::GetScaleF(testTransform, testScale);
				testCollider.extent.x *= testScale.x;
				testCollider.extent.y *= testScale.y;
				testCollider.extent.z *= testScale.z;

				// Set rotation
				GQUATERNIONF testQuat;
				GQuaternion::SetByMatrixF(testTransform, testQuat);
				GQuaternion::MultiplyQuaternionF(testCollider.rotation, testQuat, testCollider.rotation);

				GCollision::GCollisionCheck result;
				GCollision::TestOBBToOBBF(actingCollider, testCollider, result);
				if (result == GCollision::GCollisionCheck::COLLISION)
				{
					// A collison has happened

					// Bullet to Wall
					if (registry.all_of<Bullet>(*i) && registry.all_of<Obstacle>(*j))
					{
						registry.emplace_or_replace<ToDestroy>(*i);
					}
					if (registry.all_of<Bullet>(*j) && registry.all_of<Obstacle>(*i))
					{
						registry.emplace_or_replace<ToDestroy>(*j);
					}

					// Enemy to wall
					if (registry.all_of<Enemy>(*i) && registry.all_of<Obstacle>(*j))
					{
						auto& vel = registry.get<Velocity>(*i).velocity;
						BounceEnemy(actingTransform.row4, vel, testCollider);
					}
					if (registry.all_of<Enemy>(*j) && registry.all_of<Obstacle>(*i))
					{
						auto& vel = registry.get<Velocity>(*j).velocity;
						BounceEnemy(actingTransform.row4, vel, testCollider);
					}

					// Bullet to Enemy
					if (registry.all_of<Bullet>(*i) && registry.all_of<Enemy>(*j))
					{
						registry.emplace_or_replace<ToDestroy>(*i);
						registry.get<GAME::Health>(*j).hitpoints -= 1;
					}

					// Enemy to Player
					if (registry.any_of<Enemy>(*i) && registry.any_of<Player>(*j))
					{
						if (!registry.any_of<Invulnerable>(*j))
						{
							auto& hp = registry.get<Health>(*j).hitpoints;
							hp--;
							std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;
							registry.emplace<Invulnerable>(*j, config->at(UTIL::PlayerName).at("invulnPeriod").as<double>());
							std::cout << "Player's HP: " << hp << '\n';
						}
					}
				}
			}
		}
	}

	void CheckAllEnemies(entt::registry& registry)
	{
		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;
		auto& enemies = registry.view<GAME::Enemy>();

		for (auto& enemy : enemies)
		{
			if (registry.get<GAME::Health>(enemy).hitpoints <= 0)
			{
				registry.emplace<GAME::ToDestroy>(enemy);
				// store parent transform ref
				auto enemyTransform = registry.try_get<GAME::Transform>(enemy);
				GW::MATH::GMATRIXF* parentTransform;
				if (enemyTransform)
				{
					 parentTransform = &enemyTransform->transform;
				}

				// shatter
				GAME::Shatters* shatterRef = registry.try_get<GAME::Shatters>(enemy);
				if (shatterRef)
				{
					int splitAmt = shatterRef->shatterAmount;
					for (int i = 0; i < splitAmt; i++)
					{
						auto toMakeEnemy = registry.create();
						registry.emplace<GAME::Enemy>(toMakeEnemy);
						UTIL::CreateModelEntity(registry, toMakeEnemy, UTIL::Enemy1Name, parentTransform);

						// Speed & Velocity
						{
							float enemySpeed = config->at(UTIL::Enemy1Name).at("speed").as<float>();
							auto& velocity = UTIL::GetRandomVelocityVector();
							GW::MATH::GVector::ScaleF(velocity, enemySpeed, velocity);
							registry.emplace<GAME::Velocity>(toMakeEnemy, velocity);
						}

						// Update Transform
						{
							auto enemyTransform = registry.try_get<GAME::Transform>(toMakeEnemy);
							if (enemyTransform)
							{
								enemyTransform->transform = *parentTransform;

								GW::MATH::GVECTORF scaleVec = { shatterRef->shatterScale, shatterRef->shatterScale ,
																shatterRef->shatterScale ,shatterRef->shatterScale };
								GW::MATH::GMatrix::ScaleLocalF(enemyTransform->transform, scaleVec, enemyTransform->transform);
							}
						}

						// Collider
						{
							registry.emplace<GAME::Collidable>(toMakeEnemy);
							std::string name = config->at(UTIL::Enemy1Name).at("model").as<std::string>();
							auto& coll = registry.ctx().get<DRAW::ModelManager>().models[name].collider;
							registry.get<DRAW::MeshCollection>(toMakeEnemy).collider = coll;
						}

						// Stats
						registry.emplace<GAME::Health>(toMakeEnemy, config->at(UTIL::Enemy1Name).at("hitpoints").as<int>());

						// Update Shatter
						if (shatterRef->initialShatterCount > 0)
						{
							GAME::Shatters copyShatter = *shatterRef;
							--copyShatter.initialShatterCount;
							registry.emplace<GAME::Shatters>(toMakeEnemy, copyShatter);
						}
					}
				}
			}
		}
	}

	void DestroyMarkedEntites(entt::registry& registry)
	{
		// Destroy entites that are tagged with ToDestroy
		auto& toDestroyEntites = registry.group<ToDestroy>();
		for (auto entity : toDestroyEntites)
		{
			registry.destroy(entity);
		}
	}

	void CheckAllPlayersHP(entt::registry& registry, entt::entity entity)
	{
		auto allPlayers = registry.view<Player>();
		int playersHP = 0;

		for (auto currentPlayer : allPlayers)
		{
			playersHP += registry.get<Health>(currentPlayer).hitpoints;
		}

		if (playersHP <= 0)
		{
			registry.emplace<GameOver>(entity);
			std::cout << "You Lose. Game Over!\n";
		}
		else if (registry.view<Enemy>().size() == 0)
		{
			registry.emplace<GameOver>(entity);
			std::cout << "You Win. Good Job!\n";
		}

	}

	void UpdateGameManager(entt::registry& registry, entt::entity entity)
	{
		if (registry.any_of<GameOver>(entity))
			return;

		// All gameplay systems in here

		// Update Velocities and Transforms
		UpdateEntityVelocities(registry);
		UpdateMeshTransforms(registry);

		// Handle Collisions
		HandleCollisions(registry);

		// Update and Check all enemies for stats
		CheckAllEnemies(registry);

		auto allPlayers = registry.group<Player>();
		for (auto currentPlayer : allPlayers)
		{
			registry.patch<Player>(currentPlayer);
		}

		CheckAllPlayersHP(registry, entity);

		DestroyMarkedEntites(registry);
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<GameManager>().connect<UpdateGameManager>();
	}
} // namespace DRAW