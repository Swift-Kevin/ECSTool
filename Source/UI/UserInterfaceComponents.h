#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "vulkan\vulkan.hpp"

namespace UI
{
	/* Tags */
	struct UI_MenuBar {};
	struct UI_ViewEntites {};
	struct UI_ViewComponents {};
	struct UI_ViewConsole {};
	
	/* Components */
	struct UIData
	{
		VkDescriptorPool uiDescriptorPool;
	};

} // namespace UI
#endif // !UI_COMPONENTS_H
