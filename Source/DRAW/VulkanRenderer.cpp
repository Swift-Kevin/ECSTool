#include "DrawComponents.h"
#include "../CCL.h"
// component dependencies
#include "./Utility/FileIntoString.h"

#include "./Utility/VulkanHelpers.h"

#include "shaderc/shaderc.h" // needed for compiling shaders at runtime
#ifdef _WIN32 // must use MT platform DLL libraries on windows
#pragma comment(lib, "shaderc_combined.lib") 
#endif

namespace DRAW
{
	//*** HELPER METHODS ***//

	VkViewport CreateViewportFromWindowDimensions(unsigned int windowWidth, unsigned int windowHeight)
	{
		VkViewport retval = {};
		retval.x = 0;
		retval.y = 0;
		retval.width = static_cast<float>(windowWidth);
		retval.height = static_cast<float>(windowHeight);
		retval.minDepth = 0;
		retval.maxDepth = 1;
		return retval;
	}

	VkRect2D CreateScissorFromWindowDimensions(unsigned int windowWidth, unsigned int windowHeight)
	{
		VkRect2D retval = {};
		retval.offset.x = 0;
		retval.offset.y = 0;
		retval.extent.width = windowWidth;
		retval.extent.height = windowHeight;
		return retval;
	}

	void InitializeDescriptorLayouts(DRAW::VulkanRenderer& _renderer)
	{
		VkDescriptorSetLayoutBinding layoutBinding[2] =
		{
			VKH::CreateVkDescriptorSetLayoutBinding(0, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
			VKH::CreateVkDescriptorSetLayoutBinding(1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1)
		};

		VkDescriptorSetLayoutCreateInfo setCreateInfo = VKH::CreateVkDescriptorSetLayoutCreateInfo(layoutBinding, ARRAYSIZE(layoutBinding));
		vkCreateDescriptorSetLayout(_renderer.device, &setCreateInfo, nullptr, &_renderer.descriptorLayout);
	}

	void InitializeDescriptorPool(DRAW::VulkanRenderer& _renderer, unsigned _frameCount)
	{
		VkDescriptorPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		VkDescriptorPoolSize descriptorpool_size[2] = 
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _frameCount },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, _frameCount }
		};
		poolCreateInfo.poolSizeCount = 2;
		poolCreateInfo.pPoolSizes = descriptorpool_size;
		poolCreateInfo.maxSets = _frameCount;
		poolCreateInfo.flags = 0;
		poolCreateInfo.pNext = nullptr;
		vkCreateDescriptorPool(_renderer.device, &poolCreateInfo, nullptr, &_renderer.descriptorPool);
	}

	void AllocateDescriptorSets(DRAW::VulkanRenderer& _renderer, unsigned _frameCount)
	{
		VkDescriptorSetAllocateInfo allocateInfo = VKH::CreateVkDescriptorSetAllocateInfo(1, &_renderer.descriptorLayout, _renderer.descriptorPool);

		for (int i = 0; i < _frameCount; i++)
		{
			vkAllocateDescriptorSets(_renderer.device, &allocateInfo, &_renderer.descriptorSets[i]);
		}
	}

	void InitializeDescriptors(entt::registry& registry, entt::entity entity)
	{
		auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);

		unsigned int frameCount;
		vulkanRenderer.vlkSurface.GetSwapchainImageCount(frameCount);
		vulkanRenderer.descriptorSets.resize(frameCount);

		InitializeDescriptorLayouts(vulkanRenderer);
		InitializeDescriptorPool(vulkanRenderer, frameCount);
		AllocateDescriptorSets(vulkanRenderer, frameCount);

		// Add the 2 buffers, this will create the initial buffers so we can finish building our descriptor set
		auto& storageBuffer = registry.emplace<VulkanGPUInstanceBuffer>(entity,
			VulkanGPUInstanceBuffer{ 16 }); // Start with a reasonable size of elements. The Buffer will grow if it needs to later
		auto& uniformBuffer = registry.emplace<VulkanUniformBuffer>(entity);


		for (int i = 0; i < frameCount; i++)
		{
			VkDescriptorBufferInfo buffer_info[] =
			{
				VKH::CreateVkDescriptorBufferInfo(uniformBuffer.buffer[i]),
				VKH::CreateVkDescriptorBufferInfo(storageBuffer.buffer[i])
			};

			VkWriteDescriptorSet writes[] =
			{
				VKH::CreateVkWriteDescriptorSet(vulkanRenderer.descriptorSets[i], 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &buffer_info[0], VK_NULL_HANDLE, VK_NULL_HANDLE),
				VKH::CreateVkWriteDescriptorSet(vulkanRenderer.descriptorSets[i], 1, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &buffer_info[1], VK_NULL_HANDLE, VK_NULL_HANDLE)
			};

			vkUpdateDescriptorSets(vulkanRenderer.device, ARRAYSIZE(writes), writes, 0, nullptr);
		}
	}

	void InitializeGraphicsPipeline(entt::registry& registry, entt::entity entity)
	{
		auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);
		GW::SYSTEM::GWindow win = registry.get<GW::SYSTEM::GWindow>(entity);

		// Create Pipeline & Layout (Thanks Tiny!)
		VkPipelineShaderStageCreateInfo stageCreateInfo[2] = {};
		// Create Stage Info for Vertex Shader
		stageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stageCreateInfo[0].module = vulkanRenderer.vertexShader;
		stageCreateInfo[0].pName = "main";

		// Create Stage Info for Fragment Shader
		stageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stageCreateInfo[1].module = vulkanRenderer.fragmentShader;
		stageCreateInfo[1].pName = "main";

		VkPipelineInputAssemblyStateCreateInfo asmCreateInfo = {};
		asmCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		asmCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		asmCreateInfo.primitiveRestartEnable = false;

		VkVertexInputBindingDescription vtxBindDesc[4] =
		{
			VKH::CreateVkVertexInputBindingDescription(0, (sizeof(float) * 3), VK_VERTEX_INPUT_RATE_VERTEX),
			VKH::CreateVkVertexInputBindingDescription(1, (sizeof(float) * 3), VK_VERTEX_INPUT_RATE_VERTEX),
			VKH::CreateVkVertexInputBindingDescription(2, (sizeof(float) * 2), VK_VERTEX_INPUT_RATE_VERTEX),
			VKH::CreateVkVertexInputBindingDescription(3, (sizeof(float) * 4), VK_VERTEX_INPUT_RATE_VERTEX)
		};

		VkVertexInputAttributeDescription vtxAttrDesc[4] =
		{
			VKH::CreateVkVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0),
			VKH::CreateVkVertexInputAttributeDescription(1, 1, VK_FORMAT_R32G32B32_SFLOAT, 0),
			VKH::CreateVkVertexInputAttributeDescription(2, 2, VK_FORMAT_R32G32_SFLOAT, 0),
			VKH::CreateVkVertexInputAttributeDescription(3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, 0)
		};

		VkPipelineVertexInputStateCreateInfo input_vertex_info = VKH::CreateVkPipelineVertexInputStateCreateInfo(vtxBindDesc, 4, vtxAttrDesc, 4);

		unsigned int windowWidth, windowHeight;
		win.GetClientWidth(windowWidth);
		win.GetClientHeight(windowHeight);
		VkViewport viewport = CreateViewportFromWindowDimensions(windowWidth, windowHeight);
		VkRect2D scissor = CreateScissorFromWindowDimensions(windowWidth, windowHeight);
		VkPipelineViewportStateCreateInfo viewport_create_info = VKH::CreateVkPipelineViewportStateCreateInfo(&viewport, 1, &scissor, 1);
		VkPipelineRasterizationStateCreateInfo rasterization_create_info = VKH::CreateVkPipelineRasterizationStateCreateInfo();
		VkPipelineMultisampleStateCreateInfo multisample_create_info = VKH::CreateVkPipelineMultisampleStateCreateInfo();
		VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = VKH::CreateVkPipelineDepthStencilStateCreateInfo();
		VkPipelineColorBlendAttachmentState color_blend_attachment_state = VKH::CreateVkPipelineColorBlendAttachmentState();
		VkPipelineColorBlendStateCreateInfo color_blend_create_info = VKH::CreateVkPipelineColorBlendStateCreateInfo(&color_blend_attachment_state, 1);

		// Dynamic State 
		VkDynamicState dynamic_states[2] =
		{
			// By setting these we do not need to re-create the pipeline on Resize
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamic_create_info = VKH::CreateVkPipelineDynamicStateCreateInfo(dynamic_states, 2);

		InitializeDescriptors(registry, entity);

		VkPipelineLayoutCreateInfo pipelineCI = VKH::CreateVkPipelineLayoutCreateInfo(1, &vulkanRenderer.descriptorLayout);
		vkCreatePipelineLayout(vulkanRenderer.device, &pipelineCI, nullptr, &vulkanRenderer.pipelineLayout);

		// Pipeline State... (FINALLY) 
		VkGraphicsPipelineCreateInfo pipeline_create_info = {};
		pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_create_info.stageCount = 2;
		pipeline_create_info.pStages = stageCreateInfo;
		pipeline_create_info.pInputAssemblyState = &asmCreateInfo;
		pipeline_create_info.pVertexInputState = &input_vertex_info;
		pipeline_create_info.pViewportState = &viewport_create_info;
		pipeline_create_info.pRasterizationState = &rasterization_create_info;
		pipeline_create_info.pMultisampleState = &multisample_create_info;
		pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
		pipeline_create_info.pColorBlendState = &color_blend_create_info;
		pipeline_create_info.pDynamicState = &dynamic_create_info;
		pipeline_create_info.layout = vulkanRenderer.pipelineLayout;
		pipeline_create_info.renderPass = vulkanRenderer.renderPass;
		pipeline_create_info.subpass = 0;
		pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;

		vkCreateGraphicsPipelines(vulkanRenderer.device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &vulkanRenderer.pipeline);
	}

	//*** SYSTEMS ***//

	// run this code when a VulkanRenderer component is connected
	void Construct_VulkanRenderer(entt::registry& registry, entt::entity entity)
	{
		if (!registry.all_of<GW::SYSTEM::GWindow>(entity))
		{
			std::cout << "Window not added to the registry yet!" << std::endl;
			abort();
			return;
		}

		if (!registry.all_of<VulkanRendererInitialization>(entity))
		{
			std::cout << "Initialization Data not added to the registry yet!" << std::endl;
			abort();
			return;
		}

		auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);
		auto& initializationData = registry.get<VulkanRendererInitialization>(entity);

		GW::SYSTEM::GWindow win = registry.get<GW::SYSTEM::GWindow>(entity);
#ifndef NDEBUG
		const char* debugLayers[] = {
			"VK_LAYER_KHRONOS_validation", // standard validation layer
		};
		if (-vulkanRenderer.vlkSurface.Create(win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT | GW::GRAPHICS::BINDLESS_SUPPORT,
			sizeof(debugLayers) / sizeof(debugLayers[0]),
			debugLayers, 0, nullptr, 0, nullptr, false))
#else
		if (-vulkanRenderer.vlkSurface.Create(win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT))
#endif
		{
			std::cout << "Failed to create Vulkan Surface!" << std::endl;
			abort();
			return;
		}

		vulkanRenderer.clrAndDepth[0].color = initializationData.clearColor;
		vulkanRenderer.clrAndDepth[1].depthStencil = initializationData.depthStencil;

		// Create Projection matrix
		float aspectRatio;
		vulkanRenderer.vlkSurface.GetAspectRatio(aspectRatio);
		GW::MATH::GMatrix::ProjectionVulkanLHF(G_DEGREE_TO_RADIAN_F(initializationData.fovDegrees), aspectRatio, initializationData.nearPlane, initializationData.farPlane, vulkanRenderer.projMatrix);


		vulkanRenderer.vlkSurface.GetDevice((void**)&vulkanRenderer.device);
		vulkanRenderer.vlkSurface.GetPhysicalDevice((void**)&vulkanRenderer.physicalDevice);
		vulkanRenderer.vlkSurface.GetRenderPass((void**)&vulkanRenderer.renderPass);

		// Intialize runtime shader compiler HLSL -> SPIRV
		shaderc_compiler_t compiler = shaderc_compiler_initialize();
		shaderc_compile_options_t options = shaderc_compile_options_initialize();
		shaderc_compile_options_set_source_language(options, shaderc_source_language_hlsl);
		shaderc_compile_options_set_invert_y(options, false);
#ifndef NDEBUG
		shaderc_compile_options_set_generate_debug_info(options);
#endif

		// Vertex Shader
		std::string vertexShaderSource = ReadFileIntoString(initializationData.vertexShaderName.c_str());

		shaderc_compilation_result_t result = shaderc_compile_into_spv( // compile
			compiler, vertexShaderSource.c_str(), vertexShaderSource.length(),
			shaderc_vertex_shader, "main.vert", "main", options);

		if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
		{
			std::cout << "Vertex Shader Errors : \n" << shaderc_result_get_error_message(result) << std::endl;
			abort();
			return;
		}

		GvkHelper::create_shader_module(vulkanRenderer.device, shaderc_result_get_length(result), // load into Vulkan
			(char*)shaderc_result_get_bytes(result), &vulkanRenderer.vertexShader);

		shaderc_result_release(result); // done

		// Fragment Shader
		std::string fragmentShaderSource = ReadFileIntoString(initializationData.fragmentShaderName.c_str());

		result = shaderc_compile_into_spv( // compile
			compiler, fragmentShaderSource.c_str(), fragmentShaderSource.length(),
			shaderc_fragment_shader, "main.frag", "main", options);

		if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
		{
			std::cout << "Fragment Shader Errors : \n" << shaderc_result_get_error_message(result) << std::endl;
			abort();
			return;
		}

		GvkHelper::create_shader_module(vulkanRenderer.device, shaderc_result_get_length(result), // load into Vulkan
			(char*)shaderc_result_get_bytes(result), &vulkanRenderer.fragmentShader);

		shaderc_result_release(result); // done

		// Free runtime shader compiler resources
		shaderc_compile_options_release(options);
		shaderc_compiler_release(compiler);

		InitializeGraphicsPipeline(registry, entity);

		// Remove the initializtion data as we no longer need it
		registry.remove<VulkanRendererInitialization>(entity);

	}

	// run this code when a VulkanRenderer component is updated
	void Update_VulkanRenderer(entt::registry& registry, entt::entity entity)
	{
		auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);

		if (-vulkanRenderer.vlkSurface.StartFrame(2, vulkanRenderer.clrAndDepth))
		{
			std::cout << "Failed to start frame!" << std::endl;
			return;
		}

		auto win = registry.get<GW::SYSTEM::GWindow>(entity);
		unsigned int frame;
		vulkanRenderer.vlkSurface.GetSwapchainCurrentImage(frame);

		VkCommandBuffer commandBuffer;
		unsigned int currentBuffer;
		vulkanRenderer.vlkSurface.GetSwapchainCurrentImage(currentBuffer);
		vulkanRenderer.vlkSurface.GetCommandBuffer(currentBuffer, (void**)&commandBuffer);

		unsigned int windowWidth, windowHeight;
		win.GetClientWidth(windowWidth);
		win.GetClientHeight(windowHeight);
		VkViewport viewport = CreateViewportFromWindowDimensions(windowWidth, windowHeight);
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = CreateScissorFromWindowDimensions(windowWidth, windowHeight);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanRenderer.pipeline);

		// Update uniform and storage buffers
		registry.patch<VulkanUniformBuffer>(entity);

		// Get and Sort the instances
		auto instances = registry.group<GeometryData>(entt::get<GPUInstance>, entt::exclude<DoNotRender>);
		instances.sort<GeometryData>([](const GeometryData& a, const GeometryData& b) { return a < b; });

		std::vector<GPUInstance> gpuInstances;
		std::map<GeometryData, int> geoDatas;

		for (auto [entity, geo, gpu] : instances.each())
		{
			gpuInstances.push_back(gpu);
			geoDatas[geo] += 1;
		}

		// Check for presence of the buffers first as they take a few frames before they are created
		if (registry.all_of< VulkanGeometryBuffer>(entity))
		{
			// TODO: Update buffers here before the bind of the descriptor sets
			registry.emplace<std::vector<GPUInstance>>(entity, gpuInstances);
			registry.patch<VulkanGPUInstanceBuffer>(entity);

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanRenderer.pipelineLayout, 0, 1, &vulkanRenderer.descriptorSets[currentBuffer], 0, nullptr);

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanRenderer.pipelineLayout, 1, 1, &vulkanRenderer.textureDescriptorSet, 0, nullptr);


			// TODO: Draw all the things that need drawing
			auto& geometryBuffer = registry.get<VulkanGeometryBuffer>(entity);

			if (geometryBuffer.buffer != VK_NULL_HANDLE)
			{
				int instanceCount = 0;
				for (auto [data, count] : geoDatas)
				{
					vkCmdBindIndexBuffer(commandBuffer, geometryBuffer.buffer, data.indicesOffset, VK_INDEX_TYPE_UINT16);

					VkDeviceSize offsets[] = { data.positionOffset, data.normalOffset, data.uvwOffset, data.tangentOffset };
					VkBuffer buffers[] = { geometryBuffer.buffer, geometryBuffer.buffer, geometryBuffer.buffer, geometryBuffer.buffer };
					vkCmdBindVertexBuffers(commandBuffer, 0, 4, buffers, offsets);

					vkCmdDrawIndexed(commandBuffer, data.indexCount, count, 0, 0, instanceCount);
					instanceCount += count;
				}
			}

		}

		vulkanRenderer.vlkSurface.EndFrame(true);
	}

	// run this code when a VulkanRenderer component is updated
	void Destroy_VulkanRenderer(entt::registry& registry, entt::entity entity)
	{
		auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);
		// wait till everything has completed
		vkDeviceWaitIdle(vulkanRenderer.device);
		// Remove Buffer compontents
		registry.remove<VulkanGeometryBuffer>(entity);
		registry.remove<VulkanGPUInstanceBuffer>(entity);
		registry.remove<VulkanUniformBuffer>(entity);


		vkDestroyDescriptorSetLayout(vulkanRenderer.device, vulkanRenderer.descriptorLayout, nullptr);
		vkDestroyDescriptorPool(vulkanRenderer.device, vulkanRenderer.descriptorPool, nullptr);

		// Release allocated shaders & pipeline
		vkDestroyShaderModule(vulkanRenderer.device, vulkanRenderer.vertexShader, nullptr);
		vkDestroyShaderModule(vulkanRenderer.device, vulkanRenderer.fragmentShader, nullptr);
		vkDestroyPipelineLayout(vulkanRenderer.device, vulkanRenderer.pipelineLayout, nullptr);
		vkDestroyPipeline(vulkanRenderer.device, vulkanRenderer.pipeline, nullptr);
	}

	// Use this MACRO to connect the EnTT Component Logic
	CONNECT_COMPONENT_LOGIC() {
		// register the Window component's logic
		registry.on_construct<VulkanRenderer>().connect<Construct_VulkanRenderer>();
		registry.on_update<VulkanRenderer>().connect<Update_VulkanRenderer>();
		registry.on_destroy<VulkanRenderer>().connect<Destroy_VulkanRenderer>();
	}

} // namespace DRAW