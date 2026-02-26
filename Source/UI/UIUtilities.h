#ifndef UI_UTILITIES_H_
#define UI_UTILITIES_H_

#include "../UTIL/Utilities.h"
#include "../UI/UserInterfaceComponents.h"
#include "../UTIL/Behaviors.h"

namespace UI
{
	void DrawFloatControl(const std::string& _label, float& _val, float _dragWidth, ImVec4 labelColor);
	void DrawVec3Control(const std::string& label, GW::MATH::GVECTORF& values, float resetValue = 0.0f, float columnWidth = 100.0f);

}; // namespace UI


#endif