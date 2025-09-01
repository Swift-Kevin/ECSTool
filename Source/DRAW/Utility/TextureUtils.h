#ifndef TEXTUREUTILS_H
#define TEXTUREUTILS_H

// Requires tinygltf.h and Gateware.h
namespace tinygltf
{
	struct Image;
}

// function to upload a texture to the GPU
void UploadTextureToGPU(GW::GRAPHICS::GVulkanSurface _surface, const tinygltf::Image& _img,
	VkBuffer& _outTextureBuffer, VkDeviceMemory& _outTextureMemory,
	VkImage& _outTextureImage, VkImageView& _outTextureImageView);

// same as above but can be passed a file instead
void UploadTextureToGPU(GW::GRAPHICS::GVulkanSurface _surface, const std::string& _file,
	VkBuffer& _outTextureBuffer, VkDeviceMemory& _outTextureMemory,
	VkImage& _outTextureImage, VkImageView& _outTextureImageView);

VkResult CreateSampler(GW::GRAPHICS::GVulkanSurface _surface, VkSampler& _outSampler,
	VkSamplerAddressMode _addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
	VkFilter _filter = VK_FILTER_LINEAR, float _anisotropy = 4.0f);

#endif // !TEXTUREUTILS_H