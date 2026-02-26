#include "DrawComponents.h"
#include "../CCL.h"
namespace DRAW
{
	void Construct_CPULevel(entt::registry& registry, entt::entity entity) {
		// some stuff
		CPULevel& pathRef = registry.get<CPULevel>(entity);
		GW::SYSTEM::GLog levelLog;
		levelLog.Create("DebugLog");
		levelLog.EnableConsoleLogging(true);
		bool loadRes = pathRef.gameLevel.LoadLevel(pathRef.jsonPath.c_str(), pathRef.modelFilePath.c_str(), levelLog);
		std::cout << "Loading Level Result Status: " << loadRes << '\n';
	}

	void Construct_GPULevel(entt::registry& registry, entt::entity entity) {

		// Cannot get a component off of an entity where there is no data
		// auto a = registry.get<GPULevel>(entity);

		// some stuff
		CPULevel* cpuPathRef = registry.try_get<CPULevel>(entity);
		if (!cpuPathRef)
		{
			std::cout << "CPU Path Ref not found!\n";
			return;
		}

		Level_Data& levelData = cpuPathRef->gameLevel;

		// emplace a buffer component on the entity that also has the gpu level
		registry.emplace<VulkanVertexBuffer>(entity);
		// needed for vertex buffer
		registry.emplace<std::vector<H2B::VERTEX>>(entity, levelData.levelVertices);
		// update the data
		registry.patch<VulkanVertexBuffer>(entity);
		registry.emplace<VulkanIndexBuffer>(entity);
		// needed for index buffer
		registry.emplace<std::vector<unsigned int>>(entity, levelData.levelIndices);
		// update the data
		registry.patch<VulkanIndexBuffer>(entity);

		// Get the model managers collection
		auto& modelManagerCollection = registry.ctx().emplace<ModelManager>().models;

		// Part 3 starts here
		auto& _blenderObjects = levelData.blenderObjects;

		for (int i = 0; i < _blenderObjects.size(); i++)
		{
			// Get Current Model
			const auto currentModel = levelData.levelModels[_blenderObjects[i].modelIndex];

			MeshCollection meshCollect;
			modelManagerCollection[_blenderObjects[i].blendername].collider = levelData.levelColliders[_blenderObjects[i].modelIndex];

			if (currentModel.isCollidable)
			{
				auto collidableEntity = registry.create();
				registry.emplace<GAME::Collidable>(collidableEntity);
				registry.emplace<MeshCollection>(collidableEntity).collider = levelData.levelColliders[_blenderObjects[i].modelIndex];

				GAME::Transform trans = {};
				trans.localMatrix = levelData.levelTransforms[_blenderObjects[i].transformIndex];
				registry.emplace<GAME::Transform>(collidableEntity, trans);
				registry.emplace<GAME::Obstacle>(collidableEntity);
			}

			// Iterate through all meshes in model
			for (int j = 0; j < currentModel.meshCount; j++)
			{
				// Get current Mesh from model
				const auto mesh = levelData.levelMeshes[currentModel.meshStart + j];

				// Create Entity
				auto meshEntity = registry.create();

				// Setup Geometry Data
				GeometryData geoData;
				geoData.indexStart = currentModel.indexStart + mesh.drawInfo.indexOffset;
				geoData.indexCount = mesh.drawInfo.indexCount;
				geoData.vertexStart = currentModel.vertexStart;

				// Emplace component onto entity
				registry.emplace<GeometryData>(meshEntity, geoData);

				// Setup GPU Instance
				GPUInstance gpuInst;
				gpuInst.matData = levelData.levelMaterials[currentModel.materialStart + mesh.materialIndex].attrib;
				gpuInst.transform = levelData.levelTransforms[_blenderObjects[i].transformIndex];

				// Emplace component onto entity
				registry.emplace<GPUInstance>(meshEntity, gpuInst);

				// If this particular mesh is dynamic, dont render it
				if (currentModel.isDynamic)
				{
					registry.emplace<DoNotRender>(meshEntity);
					meshCollect.entites.push_back(meshEntity);
				}
			}

			// out of meshs' for loop
			if (currentModel.isDynamic)
			{
				modelManagerCollection[_blenderObjects[i].blendername].entites = meshCollect.entites;
			}

		}
	}

	CONNECT_COMPONENT_LOGIC() {
		// registry.on_construct<VulkanGPUInstanceBuffer>().connect<Construct_VulkanGPUInstanceBuffer>();
		registry.on_construct<CPULevel>().connect<Construct_CPULevel>();
		registry.on_construct<GPULevel>().connect<Construct_GPULevel>();
	}
} // namespace DRAW