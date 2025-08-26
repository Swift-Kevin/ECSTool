#include "Utilities.h"
#include "../CCL.h"
namespace UTIL
{
	void CreateModelEntity(entt::registry& _registry, entt::entity _entity, std::string _modelFromIni, GW::MATH::GMATRIXF* _transform)
	{
		std::shared_ptr<const GameConfig> config = _registry.ctx().get<UTIL::Config>().gameConfig;

		// Get current meshs on entity
		auto& meshesOnEntity = _registry.emplace<DRAW::MeshCollection>(_entity).meshs;

		// Get current model name from passed in entity name
		std::string modelName = config.get()->at(_modelFromIni).at("model").as<std::string>();
		// Store the meshes from that model as a reference
		auto& modelsMeshs = _registry.ctx().get<DRAW::ModelManager>().collection[modelName].meshs;

		// Emplace the transform and pass in the meshs from the GPU data
		if (_transform) {
			_registry.emplace<GAME::Transform>(_entity, *_transform);
		}
		else {
			_registry.emplace<GAME::Transform>(_entity, _registry.get<DRAW::GPUInstance>(modelsMeshs[0]).transform);
		}

		// For every mesh in the model
		for (int i = 0; i < modelsMeshs.size(); i++)
		{
			// Create a copy entity (per mesh)
			auto copyEntity = _registry.create();
			// Push it back into the mesh collection we have on our entity
			meshesOnEntity.push_back(copyEntity);

			// Fix transform if overridden
			DRAW::GPUInstance gpu = _registry.get<DRAW::GPUInstance>(modelsMeshs[i]);
			gpu.transform = _transform ? *_transform : gpu.transform = _registry.get<DRAW::GPUInstance>(modelsMeshs[0]).transform;

			// Emplace the same GPUInst and GeoData (since its the same mesh)
			_registry.emplace<DRAW::GPUInstance>(copyEntity, gpu);
			_registry.emplace<DRAW::GeometryData>(copyEntity, _registry.get<DRAW::GeometryData>(modelsMeshs[i]));
		}
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