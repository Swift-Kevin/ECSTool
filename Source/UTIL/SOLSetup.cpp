#include "SOLSetup.h"
#include "../CCL.h"
#include "Utilities.h"

namespace SOL
{
	entt::entity CreateMoon(entt::registry& registry, entt::entity parentE, float radius, std::string _name)
	{
		// Creating the Earth's Moon Orbiting Earth
		auto moonEntity = registry.create();
		UTIL::CreateModelEntity(registry, moonEntity, "Moon");
		registry.get<GAME::Inspectable>(moonEntity).name = _name;

		auto& child = registry.get<GAME::Transform>(moonEntity);
		child.parentID = parentE;
		auto& parent = registry.get<GAME::Transform>(parentE);

		UTIL::UpdateWorldPosition(parent, child, radius);
		UTIL::CreateOrbiter(registry, moonEntity, parentE, "Moon");
		
		return moonEntity;
	}

	entt::entity CreatePlanet(entt::registry& registry, std::string modelName)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		auto planetEntity = registry.create();
		UTIL::CreateModelEntity(registry, planetEntity, modelName);
		UTIL::CreateOrbiter(registry, planetEntity, sunEntity, modelName);
		registry.get<GAME::Transform>(planetEntity).parentID = sunEntity;
		
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
		CreateMoon(registry, marsEntity, RANDOM::GetRandomRange(4, 6), "Phobos");
		// Create Deimos
		CreateMoon(registry, marsEntity, RANDOM::GetRandomRange(4, 6), "Deimos");
	}

	void SetupJupiter(entt::registry& registry)
	{
		entt::entity jupiterEntity = CreatePlanet(registry, "Jupiter");
		
		// Io
		CreateMoon(registry, jupiterEntity, RANDOM::GetRandomRange(6, 10), "Io");
		// Europa
		CreateMoon(registry, jupiterEntity, RANDOM::GetRandomRange(6, 10), "Europa");
		// Ganymede
		CreateMoon(registry, jupiterEntity, RANDOM::GetRandomRange(6, 10), "Ganymede");
		// Callisto
		CreateMoon(registry, jupiterEntity, RANDOM::GetRandomRange(6, 10), "Callisto");
	}

	void SetupSaturn(entt::registry& registry)
	{
		entt::entity saturnEntity = CreatePlanet(registry, "Saturn");

		// Mimas
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15), "Mimas");
		// Enceldaus
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15), "Enceldaus");
		// Tethys
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15), "Tethys");
		// Dione
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15), "Dione");
		// Rhea
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15), "Rhea");
		// Titan
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15), "Titan");
		// Hyperion
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15), "Hyperion");
		// Iapetus
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15), "Iapetus");
		// Phoebe
		CreateMoon(registry, saturnEntity, RANDOM::GetRandomRange(8, 15), "Phoebe");
	}

	void SetupUranus(entt::registry& registry)
	{
		entt::entity uranusEntity = CreatePlanet(registry, "Uranus");

		// Puck
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10), "Puck");
		// Miranda
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10), "Miranda");
		// Ariel
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10), "Ariel");
		// Umbriel
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10), "Umbriel");
		// Titania
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10), "Titania");
		// Oberon
		CreateMoon(registry, uranusEntity, RANDOM::GetRandomRange(5, 10), "Oberon");
	}

	void SetupNeptune(entt::registry& registry)
	{
		entt::entity neptuneEntity = CreatePlanet(registry, "Neptune");

		// Proteus
		CreateMoon(registry, neptuneEntity, RANDOM::GetRandomRange(5, 12), "Proteus");
		// Triton
		CreateMoon(registry, neptuneEntity, RANDOM::GetRandomRange(5, 12), "Triton");
		// Nerid
		CreateMoon(registry, neptuneEntity, RANDOM::GetRandomRange(5, 12), "Nerid");
	}

	void SetupPluto(entt::registry& registry)
	{
		entt::entity plutoEntity = CreatePlanet(registry, "Pluto");
	}
	
} // namespace SOL