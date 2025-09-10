#ifndef DRAW_COMPONENTS_H
#define DRAW_COMPONENTS_H

#include "./Utility/load_data_oriented.h"
#include "../GAME/GameComponents.h"

namespace DRAW
{
	//*** TAGS ***//
	struct DoNotRender {};
	struct GPULevel {};

	//*** COMPONENTS ***//
#pragma	region Vulkan
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
		unsigned int frameCount = 0;
	};

	struct VulkanVertexBuffer
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	struct VulkanIndexBuffer
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	struct GeometryData
	{
		unsigned int indexStart, indexCount, vertexStart;
		inline bool operator < (const GeometryData a) const {
			return indexStart < a.indexStart;
		}
	};

	struct GPUInstance
	{
		GW::MATH::GMATRIXF	transform;
		H2B::ATTRIBUTES		matData;
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
#pragma endregion

	struct Camera
	{
		GW::MATH::GMATRIXF camMatrix;
	};

	struct CPULevel {
		std::string jsonPath;
		std::string modelFilePath;
		Level_Data gameLevel;
	};

	struct MeshCollection {
		std::vector<entt::entity> entites;
		GW::MATH::GOBBF collider;
	};

	struct ModelManager {
		std::map<std::string, MeshCollection> models;
	};

} // namespace DRAW
#endif // !DRAW_COMPONENTS_H
