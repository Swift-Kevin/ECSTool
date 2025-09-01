#ifndef DRAW_COMPONENTS_H
#define DRAW_COMPONENTS_H

#include "../GAME/GameComponents.h"
#include "../../ExternalAPI/TinyGLTF/tiny_gltf.h"

namespace DRAW
{
	//*** TAGS ***//
	struct DoNotRender {};

	//*** COMPONENTS ***//
	struct VulkanRendererInitialization
	{
		std::string vertexShaderName = "";
		std::string fragmentShaderName = "";
		VkClearColorValue clearColor;
		VkClearDepthStencilValue depthStencil;
		float fovDegrees = 0.0f;
		float nearPlane = 0.0f;
		float farPlane = 0.0f;
	};

	struct TextureData
	{
		VkBuffer textureBuffer;
		VkDeviceMemory textureMemory;
		VkImage textureImage;
		VkImageView textureImageView;
	};

	struct VulkanRenderer
	{
		GW::GRAPHICS::GVulkanSurface vlkSurface;
		VkDevice device = nullptr;
		VkPhysicalDevice physicalDevice = nullptr;
		VkRenderPass renderPass;
		VkShaderModule vertexShader = nullptr;
		VkShaderModule fragmentShader = nullptr;
		VkPipeline pipeline = nullptr;
		VkPipelineLayout pipelineLayout = nullptr;
		GW::MATH::GMATRIXF projMatrix;
		VkDescriptorSetLayout descriptorLayout = nullptr;
		VkDescriptorPool descriptorPool = nullptr;
		std::vector<VkDescriptorSet> descriptorSets;
		VkClearValue clrAndDepth[2];
		VkDescriptorSetLayout textureDescriptorSetLayout = nullptr;
		VkDescriptorSet textureDescriptorSet;
		VkSampler textureSampler;
		std::vector<TextureData> textureData;
	};

	struct VulkanGeometryBuffer
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	struct GeometryData
	{
		uint64_t positionOffset;
		uint64_t uvwOffset;
		uint64_t normalOffset;
		uint64_t tangentOffset;
		uint64_t indicesOffset;

		uint64_t indexCount;
		
		inline bool operator < (const GeometryData a) const {
			return indicesOffset < a.indicesOffset;
		}
	};

	struct GPUInstance
	{
		GW::MATH::GMATRIXF	transform;
		uint32_t textureIndex;
		char padding[60];
	};

	struct VulkanGPUInstanceBuffer
	{
		unsigned long long element_count = 1;
		std::vector<VkBuffer> buffer;
		std::vector<VkDeviceMemory> memory;
	};

	struct SceneData
	{
		GW::MATH::GVECTORF sunDirection, sunColor, sunAmbient, camPos;
		GW::MATH::GMATRIXF viewMatrix, projectionMatrix;
	};

	struct VulkanUniformBuffer
	{
		std::vector<VkBuffer> buffer;
		std::vector<VkDeviceMemory> memory;
	};

	struct Camera
	{
		GW::MATH::GMATRIXF camMatrix;
	};

	struct GLTFLevel {
		std::string levelPath;
		tinygltf::Model model;
	};

} // namespace DRAW
#endif // !DRAW_COMPONENTS_H
