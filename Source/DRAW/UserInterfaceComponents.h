#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "vulkan\vulkan.hpp"

namespace UI
{
	struct UIData
	{
		ImGuiContext* context = nullptr;
		VkDescriptorPool uiDescriptorPool;
	};


} // namespace UI
#endif // !UI_COMPONENTS_H
