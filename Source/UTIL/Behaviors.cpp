#include "Behaviors.h"

namespace BEHAVIORS
{
	/// <summary>
	/// Loads all Graphics related data
	/// </summary>
	/// <param name="registry">holds all ECS info</param>
	void GraphicsBehavior(entt::registry& registry)
	{
		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

		// Add an entity to handle all the graphics data
		auto display = registry.create();

		// Placing here to reduce occurrence of a json race condition crash
		std::string jsonPath = config->at("Level1").at("levelFile").as<std::string>();
		std::string modelsPath = config->at("Level1").at("modelPath").as<std::string>();
		registry.emplace<DRAW::CPULevel>(display, DRAW::CPULevel{ jsonPath, modelsPath });

		// Emplace and initialize Window component
		int windowWidth = (*config).at("Window").at("width").as<int>();
		int windowHeight = (*config).at("Window").at("height").as<int>();
		int startX = (*config).at("Window").at("xstart").as<int>();
		int startY = (*config).at("Window").at("ystart").as<int>();
		registry.emplace<APP::Window>(display,
			APP::Window{ startX, startY, windowWidth, windowHeight, GW::SYSTEM::GWindowStyle::WINDOWEDBORDERED, "ECS Tool Demo" });


		// Create the input
		auto& input = registry.ctx().emplace<UTIL::Input>();
		auto& window = registry.get<GW::SYSTEM::GWindow>(display);
		input.bufferedInput.Create(window);
		input.immediateInput.Create(window);
		input.gamePads.Create();
		auto& pressEvents = registry.ctx().emplace<GW::CORE::GEventCache>();
		pressEvents.Create(32);
		input.bufferedInput.Register(pressEvents);
		input.gamePads.Register(pressEvents);

		// Create a transient component to initialize the Renderer
		std::string vertShader = (*config).at("Shaders").at("vertex").as<std::string>();
		std::string pixelShader = (*config).at("Shaders").at("pixel").as<std::string>();
		float nearPlane = (*config).at("Render").at("near").as<float>();
		float farPlane = (*config).at("Render").at("far").as<float>();
		float fov = (*config).at("Render").at("fov").as<float>();
		registry.emplace<DRAW::VulkanRendererInitialization>(display,
			DRAW::VulkanRendererInitialization
			{
				vertShader, pixelShader, // shader names
				{ {0, 0, 0, 1} } , // clear color
				{ 1.0f, 0u }, // depth stencil
				fov, // FOV
				nearPlane, // near
				farPlane // far
			});

		registry.emplace<DRAW::VulkanRenderer>(display);

		// Emplace GPULevel
		registry.emplace<DRAW::GPULevel>(display);

		// Register for Vulkan clean up
		GW::CORE::GEventResponder shutdown;
		shutdown.Create([&](const GW::GEvent& e)
			{
				GW::GRAPHICS::GVulkanSurface::Events event;
				GW::GRAPHICS::GVulkanSurface::EVENT_DATA data;
				if (+e.Read(event, data) && event == GW::GRAPHICS::GVulkanSurface::Events::RELEASE_RESOURCES)
				{
					registry.clear<DRAW::VulkanRenderer>();
				}
			});

		registry.get<DRAW::VulkanRenderer>(display).vlkSurface.Register(shutdown);
		registry.emplace<GW::CORE::GEventResponder>(display, shutdown.Relinquish());

		UTIL::SetupCamera(registry, display);
	}

	/// <summary>
	/// Handles creation of UI
	/// </summary>
	/// <param name="registry">holds all ECS info</param>
	void UIBehavior(entt::registry& registry)
	{
		auto uiEntity = registry.create();
		UI::UIData& uiComp = registry.emplace<UI::UIData>(uiEntity);
	}

	/// <summary>
	/// Run all gameplay updates
	/// </summary>
	/// <param name="registry">holds all ECS info</param>
	void GameplayBehavior(entt::registry& registry)
	{
		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

		// Player (invisible in game)
		{
			auto playerEntity = registry.create();
			registry.emplace<GAME::Player>(playerEntity);

			GW::MATH::GVECTORF pos = { 50, 15, -5, 1 };
			GW::MATH::GMATRIXF startingTransform = GW::MATH::GIdentityMatrixF;
			GW::MATH::GMatrix::TranslateGlobalF(startingTransform, pos, startingTransform);
			GW::MATH::GMatrix::RotateYLocalF(startingTransform, G_DEGREE_TO_RADIAN_F(-60), startingTransform);
			GW::MATH::GMatrix::RotateXLocalF(startingTransform, G_DEGREE_TO_RADIAN_F(15), startingTransform);

			GAME::Transform trans = {};
			trans.local = startingTransform;
			registry.emplace<GAME::Transform>(playerEntity, trans);
		}

		// Spawn the planets
		{
			auto sunEntity = registry.create();
			UTIL::CreateModelEntity(registry, sunEntity, "Sun");
			auto mercuryEntity = registry.create();
			UTIL::CreateModelEntity(registry, mercuryEntity, "Mercury");
			auto venusEntity = registry.create();
			UTIL::CreateModelEntity(registry, venusEntity, "Venus");
			auto earthEntity = registry.create();
			UTIL::CreateModelEntity(registry, earthEntity, "Earth");
			auto marsEntity = registry.create();
			UTIL::CreateModelEntity(registry, marsEntity, "Mars");
			auto jupiterEntity = registry.create();
			UTIL::CreateModelEntity(registry, jupiterEntity, "Jupiter");
			auto saturnEntity = registry.create();
			UTIL::CreateModelEntity(registry, saturnEntity, "Saturn");
			auto uranusEntity = registry.create();
			UTIL::CreateModelEntity(registry, uranusEntity, "Uranus");
			auto neptuneEntity = registry.create();
			UTIL::CreateModelEntity(registry, neptuneEntity, "Neptune");
			auto moonEntity = registry.create();
			UTIL::CreateModelEntity(registry, moonEntity, "Moon");

			// Setup moon for orbit
			{
				auto& moonTf = registry.get<GAME::Transform>(moonEntity);
				auto& earthTf = registry.get<GAME::Transform>(earthEntity);
				GW::MATH::GVECTORF offset;
				GW::MATH::GVector::SubtractVectorF(moonTf.local.row4, earthTf.world.row4, offset);
				moonTf.local = GW::MATH::GIdentityMatrixF;
				moonTf.local.row4 = offset;

				GAME::Orbit moonOrbit = {};
				moonOrbit.parent = earthEntity;
				moonOrbit.radius = 2.0f;
				moonOrbit.angularSpeed = G_DEGREE_TO_RADIAN_F(50.0f);
				moonOrbit.axis = GAME::ORBIT_AXIS::Y;
				moonOrbit.currentAngle = 0.0f;

				registry.emplace<GAME::Orbit>(moonEntity, moonOrbit);
			}
		}

		// Create Gameplay Entity to manage all gameplay systems
		{
			auto gameplayEntity = registry.create();
			registry.emplace<GAME::GameManager>(gameplayEntity);
		}
	}

	/// <summary>
	/// Application Loop
	/// </summary>
	/// <param name="registry">holds all ECS info</param>
	void MainLoopBehavior(entt::registry& registry)
	{
		int closedCount;
		auto winView = registry.view<APP::Window>();
		auto& deltaTime = registry.ctx().emplace<UTIL::DeltaTime>().dtSec;

		do
		{
			// Set the delta time
			static auto start = std::chrono::steady_clock::now();
			double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();

			start = std::chrono::steady_clock::now();
			// Cap delta time to min 30 fps.
			if (elapsed > 1.0 / 30.0) { elapsed = 1.0 / 30.0; }

			deltaTime = elapsed;

			// Update Game
			auto gameManagerGroup = registry.group<GAME::GameManager>();
			for (auto& game : gameManagerGroup)
			{
				registry.patch<GAME::GameManager>(game);
			}

			closedCount = 0;
			// find all Windows that are not closed and call "patch" to update them
			for (auto entity : winView)
			{
				if (registry.any_of<APP::WindowClosed>(entity))
				{
					++closedCount;
				}
				else
				{
					registry.patch<APP::Window>(entity);
				}
			}
		} while (winView.size() != closedCount);
	}
} // namespace BEHAVIORS