#include "Utilities.h"
#include "../CCL.h"


namespace RANDOM
{
	GW::MATH::GVECTORF GetRandomVelocityVector()
	{
		GW::MATH::GVECTORF vel = { float((rand() % 20) - 10), float((rand() % 20) - 10), float((rand() % 20) - 10) };
		if (vel.x <= 0.0f && vel.x > -1.0f)
			vel.x = -1.0f;
		else if (vel.x >= 0.0f && vel.x < 1.0f)
			vel.x = 1.0f;

		if (vel.y <= 0.0f && vel.y > -1.0f)
			vel.y = -1.0f;
		else if (vel.y >= 0.0f && vel.y < 1.0f)
			vel.y = 1.0f;

		if (vel.z <= 0.0f && vel.z > -1.0f)
			vel.z = -1.0f;
		else if (vel.z >= 0.0f && vel.z < 1.0f)
			vel.z = 1.0f;

		GW::MATH::GVector::NormalizeF(vel, vel);

		return vel;
	}

	GW::MATH::GMATRIXF GetRandomTransform(GW::MATH::GVECTORF min, GW::MATH::GVECTORF max)
	{
		GW::MATH::GMATRIXF transform = GW::MATH::GIdentityMatrixF;
		transform.row4.x = GetRandomRange(min.x, max.x);
		transform.row4.y = GetRandomRange(min.y, max.y);
		transform.row4.z = GetRandomRange(min.z, max.z);
		return transform;
	}

	float GetRandomRange(float min, float max)
	{
		return min + ((float)rand()) / (((float)RAND_MAX / (max - min)));
	}

	GW::MATH::GVECTORF GetRotationFromMatrix(GW::MATH::GMATRIXF matrix)
	{
		double r00 = matrix.row1.x, r01 = matrix.row1.y, r02 = matrix.row1.z;
		double r10 = matrix.row2.x, r11 = matrix.row2.y, r12 = matrix.row2.z;
		double r20 = matrix.row3.x, r21 = matrix.row3.y, r22 = matrix.row3.z;

		float pitch = std::asin(-r02);
		float roll, yaw;
		if (std::fabs(std::cos(pitch)) > G_EPSILON_F)
		{
			// x rot
			roll = std::atan2(r12, r22);
			// z rot
			yaw = std::atan2(r01, r00);
		}
		else
		{
			// gimbal lock
			roll = 0.0;
			yaw = std::atan2(-r10, r11);
		}

		return GW::MATH::GVECTORF{ G_RADIAN_TO_DEGREE_F(roll), G_RADIAN_TO_DEGREE_F(pitch), G_RADIAN_TO_DEGREE_F(yaw) };
		//return GW::MATH::GVECTORF{ roll, pitch, yaw };
	}

	GW::MATH::GVECTORF RandomPointInCircle(float radius, float heightModifier = 0)
	{
		float angle = GetRandomRange(0, 1) * 2.0f * G_PI_F;
		float y = heightModifier == 0 ? 0 : GetRandomRange(-heightModifier, heightModifier);

		return { radius * std::cos(angle), y, radius * std::sin(angle) };
	}
}

namespace UTIL
{
	void CreateModelEntity(entt::registry& registry, entt::entity entity, std::string _modelFromIni, GAME::Transform* transform)
	{
		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

		// Get models
		auto& meshesOnEntity = registry.emplace<DRAW::MeshCollection>(entity).entites;
		std::string modelName = config.get()->at(_modelFromIni).at("model").as<std::string>();
		auto& modelsMeshs = registry.ctx().get<DRAW::ModelManager>().models[modelName].entites;

		// Use overriden transform if passed
		if (transform)
		{
			registry.emplace<GAME::Transform>(entity, *transform);
		}
		else
		{
			GAME::Transform trans = {};
			trans.world = registry.get<DRAW::GPUInstance>(modelsMeshs[0]).transform;
			GW::MATH::GVECTORF pos = { 0, 0, 0, 1 };
			pos.x = config.get()->at(_modelFromIni).at("posX").as<float>();
			pos.y = config.get()->at(_modelFromIni).at("posY").as<float>();
			pos.z = config.get()->at(_modelFromIni).at("posZ").as<float>();
			float s = config.get()->at(_modelFromIni).at("scale").as<float>();
			GW::MATH::GVECTORF scale = { s, s, s, 1 };
			GW::MATH::GMatrix::ScaleLocalF(trans.world, scale, trans.world);
			trans.world.row4 = pos;
			registry.emplace<GAME::Transform>(entity, trans);
		}

		for (entt::entity ent : modelsMeshs)
		{
			// Create entity per mesh
			auto copyEntity = registry.create();
			meshesOnEntity.push_back(copyEntity);

			// Fix transform if overridden
			DRAW::GPUInstance copyGPU = registry.get<DRAW::GPUInstance>(ent);
			copyGPU.transform = transform ? transform->world : registry.get<DRAW::GPUInstance>(ent).transform;

			registry.emplace<DRAW::GPUInstance>(copyEntity, copyGPU);
			registry.emplace<DRAW::GeometryData>(copyEntity, registry.get<DRAW::GeometryData>(ent));
		}
	}

	void SetupCamera(entt::registry& registry, entt::entity entity)
	{
		// Create a camera and emplace it
		GW::MATH::GMATRIXF initialCamera = GW::MATH::GIdentityMatrixF;
		// Inverse so it can be manipulated easier
		GW::MATH::GMatrix::InverseF(initialCamera, initialCamera);
		registry.emplace<DRAW::Camera>(entity, DRAW::Camera{ initialCamera });
	}
	void PrintVector(GW::MATH::GVECTORF toPrint)
	{
		std::cout << "Vector: {" << toPrint.x << ", " << toPrint.y << ", " << toPrint.z << ", " << toPrint.w << "}\n";
	}

	void CreateOrbiter(entt::registry& registry, entt::entity orbiter, entt::entity orbiting, std::string iniName)
	{
		auto& child = registry.get<GAME::Transform>(orbiter);
		auto parent = registry.get<GAME::Transform>(orbiting);
		registry.emplace<GAME::ChildTransform>(orbiter, orbiting);

		// Compute Child Local
		GW::MATH::GMATRIXF parentInverse = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMatrix::InverseF(parent.world, parentInverse);
		GW::MATH::GMatrix::MultiplyMatrixF(child.world, parentInverse, child.local);

		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;
		float rot = config.get()->at(iniName).at("rotSpeed").as<float>();

		GAME::Orbit moonOrbit = {};
		moonOrbit.parent = orbiting;
		moonOrbit.angularSpeed = G_DEGREE_TO_RADIAN_F(rot);
		moonOrbit.axis = GAME::ORBIT_AXIS::Y;
		moonOrbit.currentAngle = 0.0f;

		registry.emplace<GAME::Orbit>(orbiter, moonOrbit);
	}

	void UpdateWorldPosition(GW::MATH::GMATRIXF parentWorld, GW::MATH::GMATRIXF& childWorld, float radius)
	{
		GW::MATH::GMATRIXF parentInverse, local = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMatrix::InverseF(parentWorld, parentInverse);
		GW::MATH::GMatrix::MultiplyMatrixF(childWorld, parentInverse, local);

		// apply offset
		GW::MATH::GVECTORF pos = RANDOM::RandomPointInCircle(radius);
		parentWorld.row4.x += pos.x;
		parentWorld.row4.y += pos.y;
		parentWorld.row4.z += pos.z;

		childWorld.row4 = parentWorld.row4;
	}

	void UpdateChildren(entt::registry& registry, entt::entity parent)
	{
		auto view = registry.view<GAME::ChildTransform, GAME::Transform>();
		for (auto [entity, relation, transform] : view.each())
		{
			if (relation.parent == parent)
			{
				GW::MATH::GMatrix::MultiplyMatrixF(transform.local, registry.get<GAME::Transform>(parent).world, transform.world);
				UpdateChildren(registry, entity);
			}
		}
	}

	void ComputeHierarchy(entt::registry& registry)
	{
		auto roots = registry.view<GAME::Transform>();
		for (auto entity : roots)
		{
			UpdateChildren(registry, entity);
		}
	}

} // namespace UTIL
