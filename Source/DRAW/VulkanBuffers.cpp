#include "DrawComponents.h"
#include "../CCL.h"
#include "../../ExternalAPI/TinyGLTF/tiny_gltf.h"
#include "./Utility/TextureUtils.h"

namespace DRAW
{
	//*** HELPERS ***//
	void AddSceneData(entt::registry& registry, entt::entity entity)
	{
		auto& sceneData = registry.emplace<SceneData>(entity,
			SceneData{ { -1.0f, -1.0f, 2.0f }, { 0.9f, 0.9f, 0.9f }, { 0.2f, 0.2f, 0.2f } });

		GW::MATH::GVector::NormalizeF(sceneData.sunDirection, sceneData.sunDirection);
		auto& renderer = registry.get<VulkanRenderer>(entity);
		sceneData.projectionMatrix = renderer.projMatrix;
	}

	//*** SYSTEMS ***//
	// Forward Declare
	void Destroy_VulkanGeometryBuffer(entt::registry& registry, entt::entity entity)
	{
		if (registry.all_of<VulkanGeometryBuffer, VulkanRenderer>(entity))
		{
			auto& vkRenderer = registry.get<VulkanRenderer>(entity);
			auto& geometryBuffer = registry.get<VulkanGeometryBuffer>(entity);
			vkDeviceWaitIdle(vkRenderer.device);
			// Release allocated buffers, shaders & pipeline
			vkDestroyBuffer(vkRenderer.device, geometryBuffer.buffer, nullptr);
			vkFreeMemory(vkRenderer.device, geometryBuffer.memory, nullptr);
		}
	}

	void Update_VulkanGeometryBuffer(entt::registry& registry, entt::entity entity) 
	{
		auto& gpuBuffer = registry.get<VulkanGeometryBuffer>(entity);
		// upload the buffer to the GPU
		if (registry.all_of<VulkanRenderer, std::vector<unsigned char>>(entity)) 
		{
			// delete already attached component
			if (gpuBuffer.buffer != VK_NULL_HANDLE)
			{
				Destroy_VulkanGeometryBuffer(registry, entity);
			}

			// if there is a cpu buffer attached, lets upload it to the GPU then delete it
			auto& vkRenderer = registry.get<VulkanRenderer>(entity);
			auto& geometryData = registry.get<std::vector<unsigned char>>(entity);
			
			// Transfer triangle data to the vertex buffer. (staging would be preferred here)
			GvkHelper::create_buffer(vkRenderer.physicalDevice, vkRenderer.device, sizeof(unsigned char) * geometryData.size(),
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
				&gpuBuffer.buffer, &gpuBuffer.memory);

			GvkHelper::write_to_buffer(vkRenderer.device, gpuBuffer.memory,
				geometryData.data(), sizeof(unsigned char) * geometryData.size());

			// remove the Vertex Data
			vkDeviceWaitIdle(vkRenderer.device);
			registry.remove<std::vector<unsigned char>>(entity);
		}
	}

	void Construct_VulkanGPUInstanceBuffer(entt::registry& registry, entt::entity entity) 
	{
		auto& bufferComponent = registry.get<VulkanGPUInstanceBuffer>(entity);
		auto& renderer = registry.get<VulkanRenderer>(entity);

		unsigned int frameCount;
		renderer.vlkSurface.GetSwapchainImageCount(frameCount);
		bufferComponent.memory.resize(frameCount);
		bufferComponent.buffer.resize(frameCount);

		for (unsigned int i = 0; i < frameCount; i++)
		{
			GvkHelper::create_buffer(renderer.physicalDevice, renderer.device, sizeof(GPUInstance)*bufferComponent.element_count,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &bufferComponent.buffer[i], &bufferComponent.memory[i]);
		}
		
	}
		
	// Forward declare
	void Destroy_VulkanGPUInstanceBuffer(entt::registry& registry, entt::entity entity)
	{
		auto& gpuBuffer = registry.get<VulkanGPUInstanceBuffer>(entity);
		auto& renderer = registry.get<VulkanRenderer>(entity);

		vkDeviceWaitIdle(renderer.device);
		for (auto handle : gpuBuffer.buffer)
		{
			vkDestroyBuffer(renderer.device, handle, nullptr);
		}
		for (auto data : gpuBuffer.memory)
		{
			vkFreeMemory(renderer.device, data, nullptr);
		}
	}

	void Update_VulkanGPUInstanceBuffer(entt::registry& registry, entt::entity entity) {
		if (!registry.all_of<std::vector<GPUInstance>>(entity))
			return; // No Instances, so nothing to write. Bail

		auto& gpuBuffer = registry.get<VulkanGPUInstanceBuffer>(entity);		
		auto& instances = registry.get<std::vector<GPUInstance>>(entity);
		auto& renderer = registry.get<VulkanRenderer>(entity);

		// Resize buffer if needed
		if (instances.size() > gpuBuffer.element_count)
		{			
			Destroy_VulkanGPUInstanceBuffer(registry, entity);

			gpuBuffer.element_count *= 2; // Double the storage size if we ran out
			
			for (unsigned int i = 0; i < gpuBuffer.buffer.size(); i++)
			{
				GvkHelper::create_buffer(renderer.physicalDevice, renderer.device, sizeof(GPUInstance) * gpuBuffer.element_count,
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &gpuBuffer.buffer[i], &gpuBuffer.memory[i]);

				VkDescriptorBufferInfo storageBufferInfo = { gpuBuffer.buffer[i], 0, VK_WHOLE_SIZE };
				VkWriteDescriptorSet storageWrite = {};
				storageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				storageWrite.dstSet = renderer.descriptorSets[i];
				storageWrite.dstBinding = 1; // 1 For the storage buffer
				storageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				storageWrite.descriptorCount = 1;
				storageWrite.pBufferInfo = &storageBufferInfo;

				vkUpdateDescriptorSets(renderer.device, 1, &storageWrite, 0, nullptr);
			}
		}		

		if (instances.size() > 0)
		{
			unsigned int sizeInBytes = sizeof(GPUInstance) * instances.size();
			unsigned int frame;
			renderer.vlkSurface.GetSwapchainCurrentImage(frame);
			vkDeviceWaitIdle(renderer.device);
			GvkHelper::write_to_buffer(renderer.device, gpuBuffer.memory[frame], instances.data(), sizeInBytes);
		}

		vkDeviceWaitIdle(renderer.device);
		registry.remove<std::vector<GPUInstance>>(entity);
	}
	
	void Construct_VulkanUniformBuffer(entt::registry& registry, entt::entity entity) {

		auto& bufferComponent = registry.get<VulkanUniformBuffer>(entity);
		auto& renderer = registry.get<VulkanRenderer>(entity);

		// If there isn't a SceneData, add one
		if (!registry.all_of<SceneData>(entity))
		{
			AddSceneData(registry, entity);
		}

		unsigned int frameCount;
		renderer.vlkSurface.GetSwapchainImageCount(frameCount);
		bufferComponent.memory.resize(frameCount);
		bufferComponent.buffer.resize(frameCount);

		for (unsigned int i = 0; i < frameCount; i++)
		{
			GvkHelper::create_buffer(renderer.physicalDevice, renderer.device, sizeof(SceneData),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &bufferComponent.buffer[i], &bufferComponent.memory[i]);
		}
	}

	void Update_VulkanUniformBuffer(entt::registry& registry, entt::entity entity) {
		auto& gpuBuffer = registry.get<VulkanUniformBuffer>(entity);
		auto& renderer = registry.get<VulkanRenderer>(entity);
		auto& data = registry.get<SceneData>(entity);

		// Update Dynamic Scene Data here
		auto& camera = registry.get<Camera>(registry.view<Camera>().front());

		data.camPos = camera.camMatrix.row4;
		GW::MATH::GMatrix::InverseF(camera.camMatrix, data.viewMatrix);

		unsigned int frame;
		renderer.vlkSurface.GetSwapchainCurrentImage(frame);
		GvkHelper::write_to_buffer(renderer.device, gpuBuffer.memory[frame], &data, sizeof(SceneData));
	}

	void Destroy_VulkanUniformBuffer(entt::registry& registry, entt::entity entity) {
		auto& gpuBuffer = registry.get<VulkanUniformBuffer>(entity);
		auto& renderer = registry.get<VulkanRenderer>(entity);

		for (auto handle : gpuBuffer.buffer)
		{
			vkDestroyBuffer(renderer.device, handle, nullptr);
		}
		for (auto data : gpuBuffer.memory)
		{
			vkFreeMemory(renderer.device, data, nullptr);
		}
	}

	// Use this MACRO to connect the EnTT Component Logic
	CONNECT_COMPONENT_LOGIC() {
		// register the Window component's logic
		registry.on_update<VulkanGeometryBuffer>().connect<Update_VulkanGeometryBuffer>();
		registry.on_destroy<VulkanGeometryBuffer>().connect<Destroy_VulkanGeometryBuffer>();

		registry.on_construct<VulkanGPUInstanceBuffer>().connect<Construct_VulkanGPUInstanceBuffer>();
		registry.on_update<VulkanGPUInstanceBuffer>().connect<Update_VulkanGPUInstanceBuffer>();
		registry.on_destroy<VulkanGPUInstanceBuffer>().connect<Destroy_VulkanGPUInstanceBuffer>();

		registry.on_construct<VulkanUniformBuffer>().connect<Construct_VulkanUniformBuffer>();
		registry.on_update<VulkanUniformBuffer>().connect<Update_VulkanUniformBuffer>();
		registry.on_destroy<VulkanUniformBuffer>().connect<Destroy_VulkanUniformBuffer>();
	}

} // namespace DRAW