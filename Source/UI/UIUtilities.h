#ifndef UI_UTILITIES_H_
#define UI_UTILITIES_H_

#include "../UTIL/Utilities.h"
#include "../UI/UserInterfaceComponents.h"
#include "../UTIL/Behaviors.h"

namespace UI
{
	void DrawFloatControl(const std::string& _label, float& _val, float _dragWidth = 50, ImVec4 labelColor = ImVec4(0.1f, 0.1f, 0.3f, 1.0f));
	void DrawVec3Control(const std::string& label, GW::MATH::GVECTORF& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void DrawReadOnlyIntValue(const std::string& _label, int _val, float _dragWidth = 50, ImVec4 labelColor = ImVec4(0.1f, 0.1f, 0.3f, 1.0f));
	
	void ConstructTransformInpsector(GAME::Transform& _transform);
    void DrawEntityNode(entt::registry& registry, entt::entity entity, std::unordered_map<entt::entity, std::vector<entt::entity>>& hierarchy, UI::UIData& uiData);

}; // namespace UI


#endif