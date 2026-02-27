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
	void UpdateWorldMatrix(GAME::Transform& t, const GW::MATH::GMATRIXF* parentWorld)
	{
		if (parentWorld)
		{
			GW::MATH::GMatrix::MultiplyMatrixF(t.localMatrix, *parentWorld, t.worldMatrix);
		}
		else
		{
			t.worldMatrix = t.localMatrix;
		}
	}

	void CreateModelEntity(entt::registry& registry, entt::entity entity, std::string iniName, GAME::Transform* _transform)
	{
		auto config = registry.ctx().get<UTIL::Config>().gameConfig;
		registry.emplace<GAME::Inspectable>(entity, iniName);

		auto& meshesOnEntity = registry.emplace<DRAW::MeshCollection>(entity).entites;
		std::string modelName = config->at(iniName).at("model").as<std::string>();
		auto& modelMeshes = registry.ctx().get<DRAW::ModelManager>().models[modelName].entites;

		GAME::Transform t = {};

		if (_transform)
		{
			t = *_transform;
		}
		else
		{
			float px = config->at(iniName).at("posX").as<float>();
			float py = config->at(iniName).at("posY").as<float>();
			float pz = config->at(iniName).at("posZ").as<float>();
			float s = config->at(iniName).at("scale").as<float>();

 			t.localTranslation = { px, py, pz, 1.0f };
			t.localScale = { s,  s,  s,  0.0f };
			t.localRotation = { 0,  0,  0,  0.0f };
			t.GetLocalTransform();

			UpdateWorldMatrix(t);
		}

		registry.emplace<GAME::Transform>(entity, t);

		for (auto mesh : modelMeshes)
		{
			auto copy = registry.create();
			meshesOnEntity.push_back(copy);

			DRAW::GPUInstance gpu = registry.get<DRAW::GPUInstance>(mesh);

			gpu.transform = t.worldMatrix;

			registry.emplace<DRAW::GPUInstance>(copy, gpu);
			registry.emplace<DRAW::GeometryData>(copy, registry.get<DRAW::GeometryData>(mesh));
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

		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;
		float rot = config.get()->at(iniName).at("rotSpeed").as<float>();

		GAME::Orbit moonOrbit = {};
		moonOrbit.parent = orbiting;
		moonOrbit.angularSpeed = G_DEGREE_TO_RADIAN_F(rot);
		moonOrbit.axis = GAME::ORBIT_AXIS::Y;
		moonOrbit.currentAngle = 0.0f;

		registry.emplace<GAME::Orbit>(orbiter, moonOrbit);
	}

	void UpdateWorldPosition(GAME::Transform parentWorld, GAME::Transform& childWorld, float radius)
	{
		GW::MATH::GMATRIXF parentInverse, localMatrix = GW::MATH::GIdentityMatrixF;
		GW::MATH::GMatrix::InverseF(parentWorld.worldMatrix, parentInverse);
		GW::MATH::GMatrix::MultiplyMatrixF(childWorld.worldMatrix, parentInverse, localMatrix);

		// apply offset
		GW::MATH::GVECTORF pos = RANDOM::RandomPointInCircle(radius);
		parentWorld.localTranslation.x += pos.x;
		parentWorld.localTranslation.y += pos.y;
		parentWorld.localTranslation.z += pos.z;

		childWorld.localTranslation = parentWorld.localTranslation;
	}

	void UpdateChildren(entt::registry& registry, entt::entity parent)
	{
		auto view = registry.view<GAME::ChildTransform, GAME::Transform>();
		for (auto [entity, relation, transform] : view.each())
		{
			if (relation.parent == parent)
			{
				GW::MATH::GMatrix::MultiplyMatrixF(transform.localMatrix, registry.get<GAME::Transform>(parent).worldMatrix, transform.worldMatrix);
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

	GW::MATH::GVECTORF EulerFromQuaternion(const GW::MATH::GQUATERNIONF& _quat)
	{
		GW::MATH::GVECTORF res;
		// pseudo from https://automaticaddison.com/how-to-convert-a-quaternion-into-euler-angles-in-python/

		// Convert a quaternion into euler angles(roll, pitch, yaw)
		// roll is rotation around x in radians(counterclockwise)
		// pitch is rotation around y in radians(counterclockwise)
		// yaw is rotation around z in radians(counterclockwise)
		float t0 = +2.0 * (_quat.w * _quat.x + _quat.y * _quat.z);
		float t1 = +1.0 - 2.0 * (_quat.x * _quat.x + _quat.y * _quat.y);
		float roll_x = std::atan2(t0, t1);

		float t2 = +2.0 * (_quat.w * _quat.y - _quat.z * _quat.x);

		t2 = (t2 > +1.0) ? 1.0f : t2;
		t2 = (t2 < -1.0) ? -1.0 : t2;
		float pitch_y = std::asin(t2);

		float t3 = +2.0 * (_quat.w * _quat.z + _quat.x * _quat.y);
		float t4 = +1.0 - 2.0 * (_quat.y * _quat.y + _quat.z * _quat.z);
		float yaw_z = std::atan2(t3, t4);

		res.x = roll_x;
		res.y = pitch_y;
		res.z = yaw_z;
		res.w = 0;

		return res;
	}

} // namespace UTIL
