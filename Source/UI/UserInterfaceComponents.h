#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "vulkan\vulkan.hpp"

namespace UI
{
	enum class MenuState : byte
	{
		MenuBar,
		Entities,
		Components,
		Console
	};

	/* Tags */
	struct UI_MenuBar {};
	struct UI_ViewEntites {};
	struct UI_ViewComponents {};
	struct UI_ViewConsole {};

	/* Components */
	struct UIData
	{
		VkDescriptorPool uiDescriptorPool;
		MenuState state = MenuState::MenuBar;
		ImGuiIO* io = nullptr;
	};

} // namespace UI
#endif // !UI_COMPONENTS_H
