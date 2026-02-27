#include "Behaviors.h"

namespace BEHAVIORS
{
	/// <summary>
	/// Loads all Graphics related data
	/// </summary>
	/// <param name="registry">holds all ECS info</param>
	void InitializeGraphics(entt::registry& registry)
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
	void InitializeUI(entt::registry& registry)
	{
		auto uiEntity = registry.create();
		UI::UIData& uiComp = registry.emplace<UI::UIData>(uiEntity);
	}

	/// <summary>
	/// Run all gameplay updates
	/// </summary>
	/// <param name="registry">holds all ECS info</param>
	void InitializeGameplay(entt::registry& registry)
	{
		std::shared_ptr<const GameConfig> config = registry.ctx().get<UTIL::Config>().gameConfig;

		// Player (invisible in game)
		{
			auto playerEntity = registry.create();
			registry.emplace<GAME::Player>(playerEntity);

			GAME::Transform trans = {};
			trans.localTranslation = { 50, 15, -5, 1 };
			trans.localRotation = { 15, -60, 0, 0 };
			trans.localScale = { 1, 1, 1, 1 };

			registry.emplace<GAME::Transform>(playerEntity, trans);
			registry.emplace<GAME::Inspectable>(playerEntity, "Player");
		}

		// Spawn the planets
		{
			auto sunEntity = registry.create();
			UTIL::CreateModelEntity(registry, sunEntity, "Sun");
			registry.emplace<GAME::Sun>(sunEntity);
		}

		// Setup Planets
		{
			SOL::SetupMercury(registry);
			SOL::SetupVenus(registry);
			SOL::SetupEarth(registry);
			SOL::SetupMars(registry);
			SOL::SetupJupiter(registry);
			SOL::SetupSaturn(registry);
			SOL::SetupUranus(registry);
			SOL::SetupNeptune(registry);
			SOL::SetupPluto(registry);
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
		auto& time = registry.ctx().emplace<UTIL::DeltaTime>();

		UTIL::DebugInfo& dbg = registry.ctx().emplace<UTIL::DebugInfo>();
		dbg.debugMode = UTIL::DebugHierarchy::BaseRender;

		do
		{
			// Set the delta time
			static auto start = std::chrono::steady_clock::now();
			double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
			start = std::chrono::steady_clock::now();

			time.dtSec = elapsed;
			time.totalTime += elapsed;
			dbg.theta = std::cos(std::sin(time.totalTime));

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