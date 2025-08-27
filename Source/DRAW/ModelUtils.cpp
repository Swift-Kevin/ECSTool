#include "DrawComponents.h"
#include "../CCL.h"

namespace DRAW
{
	void DestroyMeshCollection(entt::registry& registry, entt::entity entity)
	{
		auto& meshCollections = registry.get<MeshCollection>(entity).entites;

		for (entt::entity thing : meshCollections)
		{
			registry.destroy(thing);
		}
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_destroy<MeshCollection>().connect<DestroyMeshCollection>();
	}
} // namespace DRAW