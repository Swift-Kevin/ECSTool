#include "SOLSetup.h"
#include "../CCL.h"
#include "Utilities.h"

namespace SOL
{
	void SetupMercury(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		auto mercuryEntity = registry.create();
		UTIL::CreateModelEntity(registry, mercuryEntity, "Mercury");
		UTIL::CreateOrbiter(registry, mercuryEntity, sunEntity, "Mercury");
	}

	void SetupVenus(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		auto venusEntity = registry.create();
		UTIL::CreateModelEntity(registry, venusEntity, "Venus");
		UTIL::CreateOrbiter(registry, venusEntity, sunEntity, "Venus");
	}

	void SetupEarth(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Create the Earth... i just know this is going to take forever
		auto earthEntity = registry.create();
		UTIL::CreateModelEntity(registry, earthEntity, "Earth");
		registry.emplace<GAME::Earth>(earthEntity);
		UTIL::CreateOrbiter(registry, earthEntity, sunEntity, "Earth");

		// Creating the Earth's Moon Orbiting Earth
		auto moonEntity = registry.create();
		UTIL::CreateModelEntity(registry, moonEntity, "Moon");
		registry.emplace<GAME::Moon>(moonEntity);

		// Create the Moon around Earth (+ fix position with it)
		auto& child = registry.get<GAME::Transform>(moonEntity);
		auto parent = registry.get<GAME::Transform>(earthEntity);
		UTIL::UpdateWorldPosition(parent.world, child.world, 5);
		UTIL::CreateOrbiter(registry, moonEntity, earthEntity, "Moon");

		// Visualize Changes on these entites
		registry.emplace<GAME::Observe>(earthEntity);
		registry.emplace<GAME::Observe>(moonEntity);
	}

	void SetupMars(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Create Mars
		auto marsEntity = registry.create();
		UTIL::CreateModelEntity(registry, marsEntity, "Mars");
		UTIL::CreateOrbiter(registry, marsEntity, sunEntity, "Mars");

		// Create Phobos and Child it to Mars
		auto phobosEntity = registry.create();
		UTIL::CreateModelEntity(registry, phobosEntity, "Moon");
		auto& phobosTransform = registry.get<GAME::Transform>(phobosEntity);
		auto parent = registry.get<GAME::Transform>(marsEntity);
		UTIL::UpdateWorldPosition(parent.world, phobosTransform.world, 3);
		UTIL::CreateOrbiter(registry, phobosEntity, marsEntity, "Moon");

		// Create Deimos
		auto deimosEntity = registry.create();
		UTIL::CreateModelEntity(registry, deimosEntity, "Moon");

		auto& deimosTransform = registry.get<GAME::Transform>(deimosEntity);
		UTIL::UpdateWorldPosition(parent.world, deimosTransform.world, 5);
		UTIL::CreateOrbiter(registry, deimosEntity, marsEntity, "Moon");

		// Visualize Changes on these entites
		registry.emplace<GAME::Observe>(phobosEntity);
		registry.emplace<GAME::Observe>(deimosEntity);
	}

	void SetupJupiter(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Create Jupiter and Orbit it around the Sun... icarus who?
		auto jupiterEntity = registry.create();
		UTIL::CreateModelEntity(registry, jupiterEntity, "Jupiter");
		UTIL::CreateOrbiter(registry, jupiterEntity, sunEntity, "Jupiter");
	}

	void SetupSaturn(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Im not going crazy
		// Saturn around the Sun
		auto saturnEntity = registry.create();
		UTIL::CreateModelEntity(registry, saturnEntity, "Saturn");
		UTIL::CreateOrbiter(registry, saturnEntity, sunEntity, "Saturn");


	}

	void SetupUranus(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Uranus around the Sun
		auto uranusEntity = registry.create();
		UTIL::CreateModelEntity(registry, uranusEntity, "Uranus");
		UTIL::CreateOrbiter(registry, uranusEntity, sunEntity, "Uranus");

	}

	void SetupNeptune(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Poseidon under the space sea (sun)
		auto neptuneEntity = registry.create();
		UTIL::CreateModelEntity(registry, neptuneEntity, "Neptune");
		UTIL::CreateOrbiter(registry, neptuneEntity, sunEntity, "Neptune");
		 
	}


} // namespace SOL