#include "Behaviors.h"

namespace BEHAVIORS
{
	// Called in main loop to update graphics behaviors
	// Responsible for Loading Level, Creating VulkanRenderer, and all VulkanInstances
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
		registry.emplace<DRAW::VulkanRendererInitialization>(display,
			DRAW::VulkanRendererInitialization{
				vertShader, pixelShader,
				{ {0.2f, 0.2f, 0.25f, 1} } , { 1.0f, 0u }, 75.f, 0.1f, 100.0f });
		registry.emplace<DRAW::VulkanRenderer>(display);

		// Emplace GPULevel
		registry.emplace<DRAW::GPULevel>(display);

		// Register for Vulkan clean up
		GW::CORE::GEventResponder shutdown;
		shutdown.Create([&](const GW::GEvent& e) {
			GW::GRAPHICS::GVulkanSurface::Events event;
			GW::GRAPHICS::GVulkanSurface::EVENT_DATA data;
			if (+e.Read(event, data) && event == GW::GRAPHICS::GVulkanSurface::Events::RELEASE_RESOURCES) {
				registry.clear<DRAW::VulkanRenderer>();
			}
			});
		registry.get<DRAW::VulkanRenderer>(display).vlkSurface.Register(shutdown);
		registry.emplace<GW::CORE::GEventResponder>(display, shutdown.Relinquish());
		
		UTIL::SetupCamera(registry, display);
	}

	// This function will be called by the main loop to update the gameplay
	// It will be responsible for updating the VulkanInstances and any other gameplay components
	void GameplayBehavior(entt::registry& registry)
	{
		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

		// Player (visible in game)
		{
			auto playerEntity = registry.create();
			registry.emplace<GAME::Player>(playerEntity);
			UTIL::CreateModelEntity(registry, playerEntity, UTIL::PlayerName);
			UTIL::SetupPlayer(registry, playerEntity);
		}

		// Enemy1 (visible in game)
		{
			auto enemyEntity = registry.create();
			registry.emplace<GAME::Enemy>(enemyEntity);
			UTIL::CreateModelEntity(registry, enemyEntity, UTIL::Enemy1Name);
			UTIL::SetupEnemy(registry, enemyEntity);
		}

		// Create Gameplay Entity to manage all gameplay systems
		{
			auto gameplayEntity = registry.create();
			registry.emplace<GAME::GameManager>(gameplayEntity);
		}
	}

	// This function will be called by the main loop to update the main loop
	// It will be responsible for updating any created windows and handling any input
	void MainLoopBehavior(entt::registry& registry)
	{
		// main loop
		int closedCount; // count of closed windows
		auto winView = registry.view<APP::Window>(); // for updating all windows
		auto& deltaTime = registry.ctx().emplace<UTIL::DeltaTime>().dtSec;
		// for updating all windows
		do {
			// Set the delta time
			static auto start = std::chrono::steady_clock::now();
			double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();

			start = std::chrono::steady_clock::now();
			// Cap delta time to min 30 fps. This will prevent too much time from simulating when dragging the window
			if (elapsed > 1.0 / 30.0) { elapsed = 1.0 / 30.0; }

			deltaTime = elapsed;

			// Update Game
			auto gameManagerGroup = registry.group<GAME::GameManager>();
			registry.patch<GAME::GameManager>(gameManagerGroup[0]);

			closedCount = 0;
			// find all Windows that are not closed and call "patch" to update them
			for (auto entity : winView) {
				if (registry.any_of<APP::WindowClosed>(entity))
					++closedCount;
				else
					registry.patch<APP::Window>(entity); // calls on_update()
			}
		} while (winView.size() != closedCount); // exit when all windows are closed
	}


} // namespace BEHAVIORS