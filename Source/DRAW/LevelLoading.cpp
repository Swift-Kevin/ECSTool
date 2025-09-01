#include "DrawComponents.h"
#include "../CCL.h"
#include "../../ExternalAPI/TinyGLTF/tiny_gltf.h"

namespace DRAW
{
	void Construct_GLTFLevel(entt::registry& registry, entt::entity entity) 
	{
		GLTFLevel& levelData = registry.get<GLTFLevel>(entity);
		tinygltf::TinyGLTF gltfLoader;
		std::string err;
		std::string warn;

		if (!gltfLoader.LoadASCIIFromFile(&levelData.model, &err, &warn, levelData.levelPath))
		{
			assert(false && err.c_str());
			return;
		}
	}

	void Update_GLTFLevel(entt::registry& registry, entt::entity entity)
	{
		auto& levelData = registry.get<GLTFLevel>(entity);
		auto& model = levelData.model;

		registry.emplace<std::vector<unsigned char>>(entity, model.buffers[0].data);
		registry.patch<VulkanGeometryBuffer>(entity);

		for (int i = 0; i < model.nodes.size(); i++)
		{
			auto node = model.nodes[i];
			auto mesh = model.meshes[node.mesh];

			auto newModel = registry.create();

			int position = model.accessors[mesh.primitives[0].attributes["POSITION"]].bufferView;
			int uwv = model.accessors[mesh.primitives[0].attributes["TEXCOORD_0"]].bufferView;
			int normal = model.accessors[mesh.primitives[0].attributes["NORMAL"]].bufferView;
			int tangent = model.accessors[mesh.primitives[0].attributes["TANGENT"]].bufferView;
			int indices = model.accessors[mesh.primitives[0].indices].bufferView;

			GeometryData geo = GeometryData{
				model.bufferViews[position].byteOffset,
				model.bufferViews[uwv].byteOffset,
				model.bufferViews[normal].byteOffset,
				model.bufferViews[tangent].byteOffset,
				model.bufferViews[indices].byteOffset,
				model.accessors[mesh.primitives[0].indices].count
			};
			registry.emplace<GeometryData>(newModel, geo);

			using namespace GW::MATH;
			GMATRIXF transform = GIdentityMatrixF;
			if (node.scale.size() > 0)
			{
				GMatrix::ScaleGlobalF(transform, GVECTORF{ (float)node.scale[0], (float)node.scale[1], (float)node.scale[2] }, transform);
			}
			if (node.rotation.size() > 0)
			{
				GMATRIXF rotation;
				GMatrix::ConvertQuaternionF(GQUATERNIONF{ (float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2], (float)node.rotation[3] }, rotation);
				GMatrix::MultiplyMatrixF(transform, rotation, transform);
			}
			if (node.translation.size() > 0)
			{
				GMatrix::TranslateGlobalF(transform, GVECTORF{ (float)node.translation[0], (float)node.translation[1], (float)node.translation[2] }, transform);
			}

			uint32_t texture = model.materials[mesh.primitives[0].material].pbrMetallicRoughness.baseColorTexture.index;


			GPUInstance gpuInst = GPUInstance{
				transform,
				texture
			};
			registry.emplace<GPUInstance>(newModel, gpuInst);

		}
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_construct<GLTFLevel>().connect<Construct_GLTFLevel>();
		registry.on_update<GLTFLevel>().connect<Update_GLTFLevel>();
	}
} // namespace DRAW