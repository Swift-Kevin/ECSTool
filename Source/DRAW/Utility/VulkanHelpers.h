namespace VKH
{
	void PrintLabeledDebugString(const char* label, const char* toPrint)
	{
		std::cout << label << toPrint << std::endl;
#if defined WIN32 //OutputDebugStringA is a windows-only function 
		OutputDebugStringA(label);
		OutputDebugStringA(toPrint);
#endif
	}

	struct GFXDeviceInfo
	{
		VkPhysicalDevice physicalDevice = nullptr;
		VkDevice device = nullptr;
	};

	struct GFXBufferMem
	{
		VkBuffer handle = nullptr;
		VkDeviceMemory data = nullptr;
	};

	VkResult CreateVkBufferBundle(const GFXDeviceInfo& creator, VkBufferUsageFlags usage,
		GFXBufferMem& buffer, const void* data, unsigned int sizeInBytes)
	{
		VkResult retval = VkResult::VK_ERROR_UNKNOWN;
		retval = GvkHelper::create_buffer(creator.physicalDevice, creator.device, sizeInBytes,
			usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&buffer.handle, &buffer.data);
		if (retval)	return retval;
		retval = GvkHelper::write_to_buffer(creator.device, buffer.data, data, sizeInBytes);
		return retval;
	}

	void CleanupVkBufferBundle(const GFXDeviceInfo& creator, GFXBufferMem& bundle)
	{
		vkDestroyBuffer(creator.device, bundle.handle, VK_NULL_HANDLE);
		vkFreeMemory(creator.device, bundle.data, VK_NULL_HANDLE);
	}

	VkPipelineInputAssemblyStateCreateInfo CreateVkPipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology)
	{
		VkPipelineInputAssemblyStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		retval.topology = topology;
		retval.primitiveRestartEnable = false;
		return retval;
	}

	VkVertexInputBindingDescription CreateVkVertexInputBindingDescription(unsigned int binding, float stride, VkVertexInputRate input)
	{
		VkVertexInputBindingDescription retval = {};
		retval.binding = binding;
		retval.stride = stride;
		retval.inputRate = input;
		return retval;
	}

	VkVertexInputAttributeDescription CreateVkVertexInputAttributeDescription(unsigned int binding, unsigned int location, VkFormat format, unsigned int offset)
	{
		VkVertexInputAttributeDescription retval = {};
		retval.binding = binding;
		retval.location = location;
		retval.format = format;
		retval.offset = offset;
		return retval;
	}

	VkPipelineVertexInputStateCreateInfo CreateVkPipelineVertexInputStateCreateInfo(
		VkVertexInputBindingDescription* bindingDescriptions, uint32_t bindingCount,
		VkVertexInputAttributeDescription* attributeDescriptions, uint32_t attributeCount)
	{
		VkPipelineVertexInputStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		retval.vertexBindingDescriptionCount = bindingCount;
		retval.pVertexBindingDescriptions = bindingDescriptions;
		retval.vertexAttributeDescriptionCount = attributeCount;
		retval.pVertexAttributeDescriptions = attributeDescriptions;
		return retval;
	}

	VkViewport CreateViewportFromWindowDimensions(float windowWidth, float windowHeight)
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

	VkRect2D CreateScissorFromWindowDimensions(float windowWidth, float windowHeight)
	{
		VkRect2D retval = {};
		retval.offset.x = 0;
		retval.offset.y = 0;
		retval.extent.width = windowWidth;
		retval.extent.height = windowHeight;
		return retval;
	}

	VkPipelineViewportStateCreateInfo CreateVkPipelineViewportStateCreateInfo(const VkViewport* viewports, uint32_t viewportCount, const VkRect2D* scissors, uint32_t scissorCount)
	{
		VkPipelineViewportStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		retval.viewportCount = viewportCount;
		retval.pViewports = viewports;
		retval.scissorCount = scissorCount;
		retval.pScissors = scissors;
		return retval;
	}

	VkPipelineRasterizationStateCreateInfo CreateVkPipelineRasterizationStateCreateInfo()
	{
		VkPipelineRasterizationStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		retval.rasterizerDiscardEnable = VK_FALSE;
		retval.polygonMode = VK_POLYGON_MODE_FILL;
		retval.lineWidth = 1.0f;
		retval.cullMode = VK_CULL_MODE_BACK_BIT;
		retval.frontFace = VK_FRONT_FACE_CLOCKWISE;
		retval.depthClampEnable = VK_FALSE;
		retval.depthBiasEnable = VK_FALSE;
		retval.depthBiasClamp = 0.0f;
		retval.depthBiasConstantFactor = 0.0f;
		retval.depthBiasSlopeFactor = 0.0f;
		return retval;
	}

	VkPipelineMultisampleStateCreateInfo CreateVkPipelineMultisampleStateCreateInfo()
	{
		VkPipelineMultisampleStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		retval.sampleShadingEnable = VK_FALSE;
		retval.rasterizationSamples = VK_SAMPLE_COUNT_8_BIT;
		retval.minSampleShading = 1.0f;
		retval.pSampleMask = VK_NULL_HANDLE;
		retval.alphaToCoverageEnable = VK_FALSE;
		retval.alphaToOneEnable = VK_FALSE;
		return retval;
	}

	VkPipelineDepthStencilStateCreateInfo CreateVkPipelineDepthStencilStateCreateInfo()
	{
		VkPipelineDepthStencilStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		retval.depthTestEnable = VK_TRUE;
		retval.depthWriteEnable = VK_TRUE;
		retval.depthCompareOp = VK_COMPARE_OP_LESS;
		retval.depthBoundsTestEnable = VK_FALSE;
		retval.minDepthBounds = 0.0f;
		retval.maxDepthBounds = 1.0f;
		retval.stencilTestEnable = VK_FALSE;
		return retval;
	}

	VkPipelineColorBlendAttachmentState CreateVkPipelineColorBlendAttachmentState()
	{
		VkPipelineColorBlendAttachmentState retval = {};
		retval.colorWriteMask = 0xF;
		retval.blendEnable = VK_FALSE;
		retval.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
		retval.dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
		retval.colorBlendOp = VK_BLEND_OP_ADD;
		retval.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		retval.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
		retval.alphaBlendOp = VK_BLEND_OP_ADD;
		return retval;
	}

	VkPipelineColorBlendStateCreateInfo CreateVkPipelineColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState* attachmentStates, uint32_t attachmentCount)
	{
		VkPipelineColorBlendStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		retval.logicOpEnable = VK_FALSE;
		retval.logicOp = VK_LOGIC_OP_COPY;
		retval.attachmentCount = attachmentCount;
		retval.pAttachments = attachmentStates;
		retval.blendConstants[0] = 0.0f;
		retval.blendConstants[1] = 0.0f;
		retval.blendConstants[2] = 0.0f;
		retval.blendConstants[3] = 0.0f;
		return retval;
	}

	VkPipelineDynamicStateCreateInfo CreateVkPipelineDynamicStateCreateInfo(VkDynamicState* dynamicStates, uint32_t dynamicStateCount)
	{
		VkPipelineDynamicStateCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		retval.dynamicStateCount = dynamicStateCount;
		retval.pDynamicStates = dynamicStates;
		return retval;
	}

	VkPipelineShaderStageCreateInfo CreateVkPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module, const char* name)
	{
		VkPipelineShaderStageCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		retval.stage = stage;
		retval.module = module;
		retval.pName = name;
		return retval;
	}

	VkDescriptorSetLayoutBinding CreateVkDescriptorSetLayoutBinding(unsigned int binding, VkShaderStageFlags flags,
		VkDescriptorType descriptorType, unsigned int descriptorCount, const VkSampler* sampler = VK_NULL_HANDLE)
	{
		VkDescriptorSetLayoutBinding retval = {};
		retval.binding = binding;
		retval.descriptorType = descriptorType;
		retval.descriptorCount = descriptorCount;
		retval.stageFlags = flags;
		retval.pImmutableSamplers = sampler;
		return retval;
	}

	VkDescriptorSetLayoutCreateInfo CreateVkDescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutBinding* bindings, unsigned int bindingCount)
	{
		VkDescriptorSetLayoutCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		retval.pNext = VK_NULL_HANDLE;
		retval.flags = 0;
		retval.bindingCount = bindingCount;
		retval.pBindings = bindings;
		return retval;
	}

	VkDescriptorPoolSize CreateVkDescriptorPoolSize(VkDescriptorType type, unsigned int descriptorCount)
	{
		VkDescriptorPoolSize retval = {};
		retval.type = type;
		retval.descriptorCount = descriptorCount;
		return retval;
	}

	VkDescriptorPoolCreateInfo CreateVkDescriptorPoolCreateInfo(unsigned int maxSets,
		unsigned int poolSizeCount, const VkDescriptorPoolSize* poolSizes)
	{
		VkDescriptorPoolCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		retval.pNext = VK_NULL_HANDLE;
		retval.flags = 0;
		retval.maxSets = maxSets;
		retval.poolSizeCount = poolSizeCount;
		retval.pPoolSizes = poolSizes;
		return retval;
	}

	VkDescriptorSetAllocateInfo CreateVkDescriptorSetAllocateInfo(unsigned int descriptorSetCount, const VkDescriptorSetLayout* layout, const VkDescriptorPool& pool)
	{
		VkDescriptorSetAllocateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		retval.pNext = VK_NULL_HANDLE;
		retval.descriptorPool = pool;
		retval.descriptorSetCount = descriptorSetCount;
		retval.pSetLayouts = layout;
		return retval;
	}

	VkDescriptorBufferInfo CreateVkDescriptorBufferInfo(const VkBuffer buffer)
	{
		VkDescriptorBufferInfo retval = {};
		retval.buffer = buffer;
		retval.offset = 0;
		retval.range = VK_WHOLE_SIZE;
		return retval;
	}

	VkWriteDescriptorSet CreateVkWriteDescriptorSet(VkDescriptorSet dstSet, unsigned int dstBinding, unsigned int dstArrayElem, unsigned int descriptorCount,
		VkDescriptorType descriptorType, const VkDescriptorBufferInfo* pBufferInfo, const VkDescriptorImageInfo* pImageInfo, const VkBufferView* pTexelBufferView)
	{
		VkWriteDescriptorSet retval = {};
		retval.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		retval.pNext = VK_NULL_HANDLE;
		retval.dstSet = dstSet;
		retval.dstBinding = dstBinding;
		retval.dstArrayElement = dstArrayElem;
		retval.descriptorCount = descriptorCount;
		retval.descriptorType = descriptorType;
		retval.pBufferInfo = pBufferInfo;
		retval.pImageInfo = pImageInfo;
		retval.pTexelBufferView = pTexelBufferView;
		return retval;
	}

	VkPipelineLayoutCreateInfo CreateVkPipelineLayoutCreateInfo(unsigned int layoutCount = 0, const VkDescriptorSetLayout* layouts = VK_NULL_HANDLE,
		unsigned int pushConstantRangeCount = 0, const VkPushConstantRange* pushConstantRanges = VK_NULL_HANDLE)
	{
		VkPipelineLayoutCreateInfo retval = {};
		retval.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		retval.setLayoutCount = layoutCount;
		retval.pSetLayouts = layouts;
		retval.pushConstantRangeCount = pushConstantRangeCount;
		retval.pPushConstantRanges = pushConstantRanges;
		return retval;
	}

}; // end VKH (Vulkan Helpers)