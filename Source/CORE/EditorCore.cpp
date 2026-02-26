#include "../CCL.h"
#include "EditorComponents.h"

namespace CORE
{
	void UpdateEditorCoreLoop(entt::registry& registry, entt::entity entity)
	{

	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_update<Editor>().connect<UpdateEditorCoreLoop>();
	}
} // namespace DRAW