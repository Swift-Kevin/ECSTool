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
		registry.emplace<GAME::Observe>(moonEntity, UTIL::GetRandomRange(0.1, 10));

		return moonEntity;
	}

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

		// Create the Moon around Earth (+ fix position with it)
		entt::entity moon = CreateMoon(registry, earthEntity, UTIL::GetRandomRange(3, 5));
		registry.emplace<GAME::Moon>(moon); // for inspector debugging

		// Visualize Changes on these entites
		registry.emplace<GAME::Observe>(earthEntity);
	}

	void SetupMars(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Create Mars
		auto marsEntity = registry.create();
		UTIL::CreateModelEntity(registry, marsEntity, "Mars");
		UTIL::CreateOrbiter(registry, marsEntity, sunEntity, "Mars");

		// Create Phobos
		CreateMoon(registry, marsEntity, UTIL::GetRandomRange(4, 6));
		// Create Deimos
		CreateMoon(registry, marsEntity, UTIL::GetRandomRange(4, 6));
	}

	void SetupJupiter(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Jupiter around the Sun
		auto jupiterEntity = registry.create();
		UTIL::CreateModelEntity(registry, jupiterEntity, "Jupiter");
		UTIL::CreateOrbiter(registry, jupiterEntity, sunEntity, "Jupiter");

		// Io
		CreateMoon(registry, jupiterEntity, UTIL::GetRandomRange(6, 10));
		// Europa
		CreateMoon(registry, jupiterEntity, UTIL::GetRandomRange(6, 10));
		// Ganymede
		CreateMoon(registry, jupiterEntity, UTIL::GetRandomRange(6, 10));
		// Callisto
		CreateMoon(registry, jupiterEntity, UTIL::GetRandomRange(6, 10));
	}

	void SetupSaturn(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Saturn around the Sun
		auto saturnEntity = registry.create();
		UTIL::CreateModelEntity(registry, saturnEntity, "Saturn");
		UTIL::CreateOrbiter(registry, saturnEntity, sunEntity, "Saturn");

		// Mimas
		CreateMoon(registry, saturnEntity, UTIL::GetRandomRange(8, 15));
		// Enceldaus
		CreateMoon(registry, saturnEntity, UTIL::GetRandomRange(8, 15));
		// Tethys
		CreateMoon(registry, saturnEntity, UTIL::GetRandomRange(8, 15));
		// Dione
		CreateMoon(registry, saturnEntity, UTIL::GetRandomRange(8, 15));
		// Rhea
		CreateMoon(registry, saturnEntity, UTIL::GetRandomRange(8, 15));
		// Titan
		CreateMoon(registry, saturnEntity, UTIL::GetRandomRange(8, 15));
		// Hyperion
		CreateMoon(registry, saturnEntity, UTIL::GetRandomRange(8, 15));
		// Iapetus
		CreateMoon(registry, saturnEntity, UTIL::GetRandomRange(8, 15));
		// Phoebe
		CreateMoon(registry, saturnEntity, UTIL::GetRandomRange(8, 15));
	}

	void SetupUranus(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Uranus around the Sun
		auto uranusEntity = registry.create();
		UTIL::CreateModelEntity(registry, uranusEntity, "Uranus");
		UTIL::CreateOrbiter(registry, uranusEntity, sunEntity, "Uranus");

		// Puck
		CreateMoon(registry, uranusEntity, UTIL::GetRandomRange(5, 10));
		// Miranda
		CreateMoon(registry, uranusEntity, UTIL::GetRandomRange(5, 10));
		// Ariel
		CreateMoon(registry, uranusEntity, UTIL::GetRandomRange(5, 10));
		// Umbriel
		CreateMoon(registry, uranusEntity, UTIL::GetRandomRange(5, 10));
		// Titania
		CreateMoon(registry, uranusEntity, UTIL::GetRandomRange(5, 10));
		// Oberon
		CreateMoon(registry, uranusEntity, UTIL::GetRandomRange(5, 10));
	}

	void SetupNeptune(entt::registry& registry)
	{
		entt::entity sunEntity = registry.view<GAME::Sun>().front();

		// Poseidon under the space sea (sun)
		auto neptuneEntity = registry.create();
		UTIL::CreateModelEntity(registry, neptuneEntity, "Neptune");
		UTIL::CreateOrbiter(registry, neptuneEntity, sunEntity, "Neptune");
		 
		// Proteus
		CreateMoon(registry, neptuneEntity, UTIL::GetRandomRange(5, 12));
		// Triton
		CreateMoon(registry, neptuneEntity, UTIL::GetRandomRange(5, 12));
		// Nerid
		CreateMoon(registry, neptuneEntity, UTIL::GetRandomRange(5, 12));
	}


} // namespace SOL