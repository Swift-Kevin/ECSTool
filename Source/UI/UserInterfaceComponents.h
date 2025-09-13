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
	COMPONENT(UI_MenuBar) {};
	COMPONENT(UI_ViewEntites) {};
	COMPONENT(UI_ViewComponents) {};
	COMPONENT(UI_ViewConsole) {};

	/* Components */
	COMPONENT(UIData)
	{
		VkDescriptorPool uiDescriptorPool;
		MenuState state = MenuState::MenuBar;
		ImGuiIO* io = nullptr;
	};

} // namespace UI
#endif // !UI_COMPONENTS_H
