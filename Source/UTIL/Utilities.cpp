#include "Utilities.h"
#include "../CCL.h"
namespace UTIL
{
	void CreateModelEntity(entt::registry& registry, entt::entity entity, std::string _modelFromIni, GW::MATH::GMATRIXF* transform)
	{
		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

		// Get models
		auto& meshesOnEntity = registry.emplace<DRAW::MeshCollection>(entity).entites;
		std::string modelName = config.get()->at(_modelFromIni).at("model").as<std::string>();
		auto& modelsMeshs = registry.ctx().get<DRAW::ModelManager>().models[modelName].entites;

		// Use overriden transform if passed
		if (transform) {
			registry.emplace<GAME::Transform>(entity, *transform);
		}
		else {
			registry.emplace<GAME::Transform>(entity, registry.get<DRAW::GPUInstance>(modelsMeshs[0]).transform);
		}

		for (entt::entity ent : modelsMeshs)
		{
			// Create entity per mesh
			auto copyEntity = registry.create();
			meshesOnEntity.push_back(copyEntity);

			// Fix transform if overridden
			DRAW::GPUInstance copyGPU = registry.get<DRAW::GPUInstance>(ent);
			copyGPU.transform = transform ? *transform : copyGPU.transform = registry.get<DRAW::GPUInstance>(ent).transform;

			registry.emplace<DRAW::GPUInstance>(copyEntity, copyGPU);
			registry.emplace<DRAW::GeometryData>(copyEntity, registry.get<DRAW::GeometryData>(ent));
		}
	}

	void SetupPlayer(entt::registry& registry, entt::entity entity)
	{

	}

	void SetupCamera(entt::registry& registry, entt::entity entity)
	{
		// Create a camera and emplace it
		GW::MATH::GMATRIXF initialCamera;
		GW::MATH::GVECTORF translate = { 0.0f,  45.0f, -5.0f };
		GW::MATH::GVECTORF lookat = { 0.0f, 0.0f, 0.0f };
		GW::MATH::GVECTORF up = { 0.0f, 1.0f, 0.0f };
		GW::MATH::GMatrix::TranslateGlobalF(initialCamera, translate, initialCamera);
		GW::MATH::GMatrix::LookAtLHF(translate, lookat, up, initialCamera);
		// Inverse to turn it into a camera matrix, not a view matrix. This will let us do
		// camera manipulation in the component easier
		GW::MATH::GMatrix::InverseF(initialCamera, initialCamera);
		registry.emplace<DRAW::Camera>(entity, DRAW::Camera{ initialCamera });
	}

	GW::MATH::GVECTORF GetRandomVelocityVector()
	{
		GW::MATH::GVECTORF vel = { float((rand() % 20) - 10), 0.0f, float((rand() % 20) - 10) };
		if (vel.x <= 0.0f && vel.x > -1.0f)
			vel.x = -1.0f;
		else if (vel.x >= 0.0f && vel.x < 1.0f)
			vel.x = 1.0f;

		if (vel.z <= 0.0f && vel.z > -1.0f)
			vel.z = -1.0f;
		else if (vel.z >= 0.0f && vel.z < 1.0f)
			vel.z = 1.0f;

		GW::MATH::GVector::NormalizeF(vel, vel);

		return vel;
	}

	void PrintVector(GW::MATH::GVECTORF toPrint)
	{
		std::cout << "Vector: {" << toPrint.x << ", " << toPrint.y << ", " << toPrint.z << ", " << toPrint.w << "}\n";
	}

} // namespace UTIL