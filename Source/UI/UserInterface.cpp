#include "../CCL.h"
#include "../UTIL/Utilities.h"
#include "../UI/UserInterfaceComponents.h"
#include "../UTIL/Behaviors.h"

static HWND    winHandle = nullptr;
static WNDPROC winProc = nullptr;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK ImGui_WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return 1;
	return CallWindowProc(winProc, hWnd, msg, wParam, lParam);
}

namespace UI
{
	void ConstructTransformInpsector(GW::MATH::GMATRIXF& matrix)
	{
		auto& rotVec = UTIL::GetRotationFromMatrix(matrix);
		float* pos[3] = { &matrix.row4.x, &matrix.row4.y, &matrix.row4.z, };
		float* rot[3] = { &rotVec.x, &rotVec.y, &rotVec.z };
		float* sca[3] = { &matrix.row1.x, &matrix.row2.y, &matrix.row3.z };

		ImGui::TextUnformatted("Position");
		ImGui::SameLine();
		ImGui::DragFloat3("##Position", *pos, 0.1f);
		ImGui::TextUnformatted("Rotation");
		ImGui::SameLine();
		ImGui::DragFloat3("##Rotation", *rot, 0.1f);
		ImGui::TextUnformatted("Scale");
		ImGui::SameLine();
		ImGui::DragFloat3("##Scale", *sca, 0.1f);
	}

	void Update_UIMenuBar(entt::registry& registry, entt::entity entity)
	{
		UI::UIData& uiData = registry.get<UI::UIData>(entity);

		// Menu Bar
		if (ImGui::BeginMainMenuBar())
		{
			uiData.menuBarSize = ImGui::GetWindowSize();

			if (ImGui::MenuItem("Entites"))
			{
				LOG::Log("Opening Entites Menu");
				uiData.state = UI::MenuState::Entities;
			}
			if (ImGui::MenuItem("Components"))
			{
				LOG::Log("Opening Components Menu");
				uiData.state = UI::MenuState::Components;
			}
			if (ImGui::BeginMenu("Hierarchy"))
			{
				auto& info = registry.ctx().get<UTIL::DebugInfo>();

				if (ImGui::MenuItem("Base Render")) { info.debugMode = UTIL::DebugHierarchy::BaseRender; }
				if (ImGui::MenuItem("Rotation")) { info.debugMode = UTIL::DebugHierarchy::Rotation; }
				if (ImGui::MenuItem("Translation")) { info.debugMode = UTIL::DebugHierarchy::Translation; }
				if (ImGui::MenuItem("Scales")) { info.debugMode = UTIL::DebugHierarchy::Scale; }
				if (ImGui::MenuItem("Combined")) { info.debugMode = UTIL::DebugHierarchy::Combined; }
				if (ImGui::MenuItem("Multi Combined")) { info.debugMode = UTIL::DebugHierarchy::SolarSystem; }

				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();

		if (uiData.state == UI::MenuState::Entities)
			registry.patch<UI::UI_ViewEntites>(entity);
		else if (uiData.state == UI::MenuState::Components)
			registry.patch<UI::UI_ViewComponents>(entity);

		registry.patch<UI::UI_Inspector>(entity);
		registry.patch<UI::UI_ViewConsole>(entity);
		registry.patch<UI::UI_StressTest>(entity);

		// Camera Text
		{
			ImGui::SetNextWindowPos(ImVec2(uiData.io->DisplaySize.x * 0.2f, uiData.menuBarSize.y));
			ImGui::SetNextWindowSize(ImVec2(175, 10));

			ImGuiWindowFlags flags = {};
			flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;
			flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
			flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
			flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;

			if (ImGui::Begin("##Controls", 0, flags))
			{
				ImGui::Text("Hold Mouse 2 - Camera");
			}
			ImGui::End();
		}
	}

	void Update_UIViewEntitiesMenu(entt::registry& registry, entt::entity entity)
	{
		ImGuiWindowFlags flags = {};
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;

		UI::UIData& uiData = registry.get<UI::UIData>(entity);
		ImGui::SetNextWindowPos(ImVec2(0, uiData.menuBarSize.y));
		ImGui::SetNextWindowSize(ImVec2(uiData.io->DisplaySize.x * 0.2f, uiData.io->DisplaySize.y - uiData.menuBarSize.y));

		if (ImGui::Begin("Entities", 0, flags))
		{
			ImVec2 availableSpace = ImGui::GetContentRegionAvail();
			if (ImGui::BeginListBox("##", availableSpace))
			{
				for (auto entity : registry.view<GAME::Transform>())
				{
					std::string name = "Entity: " + std::to_string((ENTT_ID_TYPE)entity);
					if (ImGui::Selectable(name.c_str(), false))
					{
						uiData.inspectingEntity = entity;
					}
				}
				ImGui::EndListBox();
			}
		}
		ImGui::End();
	}

	void Update_UIViewComponentsMenu(entt::registry& registry, entt::entity entity)
	{
		ImGuiWindowFlags flags = {};
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;

		UI::UIData& uiData = registry.get<UI::UIData>(entity);
		ImGui::SetNextWindowPos(ImVec2(0, uiData.menuBarSize.y));
		ImGui::SetNextWindowSize(ImVec2(uiData.io->DisplaySize.x * 0.2f, uiData.io->DisplaySize.y - uiData.menuBarSize.y));

		if (ImGui::Begin("Components", 0, flags))
		{
			ImVec2 availableSpace = ImGui::GetContentRegionAvail();
			if (ImGui::BeginListBox("##", availableSpace))
			{
				auto& map = RegisteredComponents();
				for (auto [compName, compIdx] : map)
				{
					std::string name = compName;
					ImGui::Selectable(name.c_str(), false);
				}
				ImGui::EndListBox();
			}
		}
		ImGui::End();
	}

	void Update_UIViewConsoleMenu(entt::registry& registry, entt::entity entity)
	{
		ImGuiWindowFlags flags = {};
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;

		UI::UIData& uiData = registry.get<UI::UIData>(entity);
		ImGui::SetNextWindowPos(ImVec2(uiData.io->DisplaySize.x * 0.2, uiData.io->DisplaySize.y * 0.75));
		ImGui::SetNextWindowSize(ImVec2(uiData.io->DisplaySize.x * 0.6, uiData.io->DisplaySize.y * 0.25));

		static bool showLog = true;
		static bool showWarning = true;
		static bool showError = true;
		static bool showDebug = true;

		if (ImGui::Begin("Console", 0, flags))
		{
			ImGui::Text("Filter: ");
			ImGui::SameLine();
			ImGui::Checkbox("Debug", &showDebug);
			ImGui::SameLine();
			ImGui::Checkbox("Warning", &showWarning);
			ImGui::SameLine();
			ImGui::Checkbox("Error", &showError);

			ImVec2 availableSpace = ImGui::GetContentRegionAvail();
			if (ImGui::BeginListBox("##", availableSpace))
			{
				std::vector<LOG::LogEntry> logs = registry.ctx().get<LOG::Logs>().messages;

				for (auto log : logs)
				{
					bool showMsg = LOG::ShowMessageStatus(showDebug, showWarning, showError, log.severity);

					if (showMsg)
					{
						ImVec4 color = ImVec4(1, 1, 1, 1);
						if (log.severity == LOG::LogSeverity::Warning)
						{
							color = ImVec4(1, 1, 0, 1);
						}
						else if (log.severity == LOG::LogSeverity::Error)
						{
							color = ImVec4(1, 0, 0, 1);
						}

						ImGui::TextColored(color, "%s", LOG::EnumToLabel(log.severity));
						ImGui::SameLine();
						ImGui::Text(log.content.c_str());
					}
				}

				ImGui::SetScrollHereY(1);
				ImGui::EndListBox();
			}
		}
		ImGui::End();
	}

	void Update_UIInspector(entt::registry& registry, entt::entity entity)
	{
		ImGuiWindowFlags flags = {};
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;

		UI::UIData& uiData = registry.get<UI::UIData>(entity);
		ImGui::SetNextWindowPos(ImVec2(uiData.io->DisplaySize.x - (uiData.io->DisplaySize.x * 0.2), uiData.menuBarSize.y));
		ImGui::SetNextWindowSize(ImVec2(uiData.io->DisplaySize.x * 0.2, uiData.io->DisplaySize.y * 0.25));

		if (ImGui::Begin("Inspector", 0, flags))
		{
			if (uiData.inspectingEntity != entt::null)
			{
				ImGui::PushItemWidth(200);
				GAME::Transform transform = registry.get<GAME::Transform>(uiData.inspectingEntity);
				if (ImGui::CollapsingHeader("World Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ConstructTransformInpsector(transform.world);
				}
				if (ImGui::CollapsingHeader("Local Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ConstructTransformInpsector(transform.local);
				}
				ImGui::PopItemWidth();
			}
			else
			{
				ImGui::Text("No Entity Selected");
			}
		}
		ImGui::End();
	}

	void Update_UIStressTest(entt::registry& registry, entt::entity entity)
	{
		ImGuiWindowFlags flags = {};
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoMove;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;

		UI::UIData& uiData = registry.get<UI::UIData>(entity);
		ImGui::SetNextWindowPos(ImVec2(uiData.io->DisplaySize.x - (uiData.io->DisplaySize.x * 0.2), (uiData.io->DisplaySize.y * 0.25) + uiData.menuBarSize.y));
		ImGui::SetNextWindowSize(ImVec2(uiData.io->DisplaySize.x * 0.2, uiData.io->DisplaySize.y * 0.75));

		if (ImGui::Begin("Testing Suite", 0, flags))
		{
			if (ImGui::Button("+10 Entites"))
			{
				BEHAVIORS::AddEntites(10);
			}
			if (ImGui::Button("+20 Entites"))
			{
				BEHAVIORS::AddEntites(20);
			}
			if (ImGui::Button("+30 Entites"))
			{
				BEHAVIORS::AddEntites(30);
			}
			if (ImGui::Button("+50 Entites"))
			{
				BEHAVIORS::AddEntites(50);
			}
			if (ImGui::Button("+100 Entites"))
			{
				BEHAVIORS::AddEntites(100);
			}
			if (ImGui::Button("+500 Entites"))
			{
				BEHAVIORS::AddEntites(500);
			}
			if (ImGui::Button("+1,000 Entites"))
			{
				BEHAVIORS::AddEntites(1000);
			}
			if (ImGui::Button("+10,000 Entites"))
			{
				BEHAVIORS::AddEntites(10000);
			}
			if (ImGui::Button("+25,000 Entites"))
			{
				BEHAVIORS::AddEntites(25000);
			}
		}
		ImGui::End();
	}

	void Construct_UIContext(entt::registry& registry, entt::entity entity)
	{
		DRAW::VulkanRenderer& vlk = registry.get<DRAW::VulkanRenderer>(registry.group<DRAW::VulkanRenderer>().front());
		GW::SYSTEM::GWindow& win = registry.get<GW::SYSTEM::GWindow>(registry.group<GW::SYSTEM::GWindow>().front());
		UI::UIData& uiData = registry.get<UI::UIData>(entity);

		unsigned int width = 0;
		unsigned int height = 0;
		win.GetClientWidth(width);
		win.GetClientHeight(height);

		void* instance = nullptr;
		vlk.vlkSurface.GetInstance(&instance);
		void* gfxQueue = nullptr;
		vlk.vlkSurface.GetGraphicsQueue(&gfxQueue);
		unsigned int gfxIdx = 0;
		unsigned int presentIdx = 0;
		vlk.vlkSurface.GetQueueFamilyIndices(gfxIdx, presentIdx);

		//1: create descriptor pool for IMGUI
		// the size of the pool is very oversize, but it's copied from imgui demo itself.
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		vkCreateDescriptorPool(vlk.device, &pool_info, nullptr, &uiData.uiDescriptorPool);
		// 2: initialize imgui library

		//this initializes the core structures of imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		uiData.io = &(ImGui::GetIO());
		uiData.io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		uiData.io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		uiData.io->DisplaySize = ImVec2(width, height);

		//this initializes imgui for SDL
		GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE hand;
		if (win.GetWindowHandle(hand) != GW::GReturn::SUCCESS)
		{
			assert("Failed to grab window handle.");
		}
		if (!ImGui_ImplWin32_Init(&hand))
		{
			assert("Failed to grab window handle.");
		}

		VkPipelineRenderingCreateInfoKHR pcri = {};
		pcri.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pcri.colorAttachmentCount = 1;

		//this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = (VkInstance)instance;
		initInfo.PhysicalDevice = vlk.physicalDevice;
		initInfo.Device = vlk.device;
		initInfo.Queue = (VkQueue)gfxQueue;
		initInfo.DescriptorPool = uiData.uiDescriptorPool;
		initInfo.MinImageCount = vlk.frameCount;
		initInfo.ImageCount = vlk.frameCount;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.RenderPass = vlk.renderPass;
		initInfo.PipelineRenderingCreateInfo = pcri;

		if (!ImGui_ImplVulkan_Init(&initInfo))
		{
			assert("Failed to implement vulkan startup");
		}

		if (!ImGui_ImplVulkan_CreateFontsTexture())
		{
			assert("Failed to implement vulkan startup");
		}

		// Register Window Input/Handling
		winHandle = reinterpret_cast<HWND>(hand.window);
		winProc = (WNDPROC)::GetWindowLongPtr(winHandle, GWLP_WNDPROC);
		::SetWindowLongPtr(winHandle, GWLP_WNDPROC, (LONG_PTR)ImGui_WndProcHook);

		// Emplace all UI menus so we can hide/show when neccesary
		registry.emplace<UI::UI_MenuBar>(entity);
		registry.emplace<UI::UI_ViewEntites>(entity);
		registry.emplace<UI::UI_ViewComponents>(entity);
		registry.emplace<UI::UI_ViewConsole>(entity);
		registry.emplace<UI::UI_Inspector>(entity);
		registry.emplace<UI::UI_StressTest>(entity);
	}

	void Update_UIContext(entt::registry& registry, entt::entity entity)
	{
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplVulkan_NewFrame();

		GW::SYSTEM::GWindow& win = registry.get<GW::SYSTEM::GWindow>(registry.group<GW::SYSTEM::GWindow>().front());
		unsigned int width = 0, height = 0;
		win.GetClientWidth(width);
		win.GetClientHeight(height);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(width, height);

		ImGui::NewFrame();
		// For referencing documentation:
		//ImGui::ShowDemoWindow();

		registry.patch<UI::UI_MenuBar>(entity);
	}

	CONNECT_COMPONENT_LOGIC()
	{
		// All UI Menus
		registry.on_update<UI::UI_MenuBar>().connect<Update_UIMenuBar>();
		registry.on_update<UI::UI_ViewEntites>().connect<Update_UIViewEntitiesMenu>();
		registry.on_update<UI::UI_ViewComponents>().connect<Update_UIViewComponentsMenu>();
		registry.on_update<UI::UI_ViewConsole>().connect<Update_UIViewConsoleMenu>();
		registry.on_update<UI::UI_Inspector>().connect<Update_UIInspector>();
		registry.on_update<UI::UI_StressTest>().connect<Update_UIStressTest>();

		// UI Data Component
		registry.on_construct<UI::UIData>().connect<Construct_UIContext>();
		registry.on_update<UI::UIData>().connect<Update_UIContext>();
	}

}; // namespace UI