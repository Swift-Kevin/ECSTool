#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

namespace UI
{
	enum class MenuState : byte
	{
		Entities,
		Components,
	};

	/* Tags */
	COMPONENT(UI_MenuBar) {};
	COMPONENT(UI_Inspector) {};
	COMPONENT(UI_ViewEntites) {};
	COMPONENT(UI_ViewComponents) {};
	COMPONENT(UI_ViewConsole) {};
	COMPONENT(UI_StressTest) {};

	/* Components */
	COMPONENT(UIData)
	{
		VkDescriptorPool uiDescriptorPool;
		MenuState state = MenuState::Entities;
		ImGuiIO* io = nullptr;
		entt::entity inspectingEntity = entt::null;
		ImVec2 menuBarSize = ImVec2(0, 0);
	};

} // namespace UI
#endif // !UI_COMPONENTS_H
