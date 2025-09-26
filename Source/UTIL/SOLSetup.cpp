#include "SOLSetup.h"
#include "../CCL.h"
#include "Utilities.h"

namespace SOL
{
	entt::entity CreateMoon(entt::registry& registry, entt::entity parentE, float radius)
	{
		// Creating the Earth's Moon Orbiting Earth
		auto moonEntity = registry.create();
		UTIL::CreateModelEntity(registry, moonEntity, "Moon");

		auto& child = registry.get<GAME::Transform>(moonEntity);
		auto parent = registry.get<GAME::Transform>(parentE);
		UTIL::UpdateWorldPosition(parent.world, child.world, radius);
		UTIL::CreateOrbiter(registry, moonEntity, parentE, "Moon");
		
		return moonEntity;
	}

	entt::entity CreatePlanet(entt::registry& registry, std::string modelName)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		auto planetEntity = registry.create();
		UTIL::CreateModelEntity(registry, planetEntity, modelName);
		UTIL::CreateOrbiter(registry, planetEntity, sunEntity, modelName);
		return planetEntity;
	}

	void SetupMercury(entt::registry& registry)
	{
		CreatePlanet(registry, "Mercury");
	}

	void SetupVenus(entt::registry& registry)
	{
		CreatePlanet(registry, "Venus");
	}

	void SetupEarth(entt::registry& registry)
	{
		entt::entity earthEntity = CreatePlanet(registry, "Earth");
		registry.emplace<GAME::Earth>(earthEntity);

		entt::entity moon = CreateMoon(registry, earthEntity, RANDOM::GetRandomRange(3, 5));
	}

	void SetupMars(entt::registry& registry)
	{
		entt::entity marsEntity = CreatePlanet(registry, "Mars");

		// Create Phobos
		CreateMoon(registry, marsEntity, RANDOM::GetRandomRange(4, 6));
		// Create Deimos
		CreateMoon(registry, marsEntity, RANDOM::GetRandomRange(4, 6));
	}

	void SetupJupiter(entt::registry& registry)
	{
		entt::entity jupiterEntity = CreatePlanet(registry, "Jupiter");
		
		// Io
		CreateMoon(registry, jupiterEntity, RANDOM::GetRandomRange(6, 10));
		// Europa
		CreateMoon(registry, jupiterEntity, RANDOM::GetRandomRange(6, 10));
		// Ganymede
		CreateMoon(registry, jupiterEntity, RANDOM::GetRandomRange(6, 10));
		// Callisto
		CreateMoon(registry, jupiterEntity, RANDOM::GetRandomRange(6, 10));
	}

	void SetupSaturn(entt::registry& registry)
	{
		entt::entity saturnEntity = CreatePlanet(registry, "Saturn");

		// Mimas
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15));
		// Enceldaus
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15));
		// Tethys
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15));
		// Dione
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15));
		// Rhea
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15));
		// Titan
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15));
		// Hyperion
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15));
		// Iapetus
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15));
		// Phoebe
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15));
	}

	void SetupUranus(entt::registry& registry)
	{
		entt::entity uranusEntity = CreatePlanet(registry, "Uranus");

		// Puck
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10));
		// Miranda
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10));
		// Ariel
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10));
		// Umbriel
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10));
		// Titania
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10));
		// Oberon
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10));
	}

	void SetupNeptune(entt::registry& registry)
	{
		entt::entity neptuneEntity = CreatePlanet(registry, "Neptune");

		// Proteus
		CreateMoon(registry, neptuneEntity, RANDOM::GetRandomRange(5, 12));
		// Triton
		CreateMoon(registry, neptuneEntity, RANDOM::GetRandomRange(5, 12));
		// Nerid
		CreateMoon(registry, neptuneEntity, RANDOM::GetRandomRange(5, 12));
	}

	void SetupPluto(entt::registry& registry)
	{
		entt::entity plutoEntity = CreatePlanet(registry, "Pluto");
	}


} // namespace SOL