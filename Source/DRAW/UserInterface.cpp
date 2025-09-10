#include "../CCL.h"
#include "../UTIL/Utilities.h"
#include "../DRAW/UserInterfaceComponents.h"

namespace UI
{
	void Construct_UIContext(entt::registry& registry, entt::entity entity)
	{
		DRAW::VulkanRenderer& vlk = registry.get<DRAW::VulkanRenderer>(registry.group<DRAW::VulkanRenderer>().front());
		GW::SYSTEM::GWindow& win = registry.get<GW::SYSTEM::GWindow>(registry.group<GW::SYSTEM::GWindow>().front());
		UI::UIData& uiCtx = registry.get<UI::UIData>(entity);

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

		vkCreateDescriptorPool(vlk.device, &pool_info, nullptr, &uiCtx.uiDescriptorPool);
		// 2: initialize imgui library

		//this initializes the core structures of imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		bool status = false;
		GW::GReturn ret;

		//this initializes imgui for SDL
		GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE hand;
		ret = win.GetWindowHandle(hand);
		status = ImGui_ImplWin32_Init(&hand);

		VkPipelineRenderingCreateInfoKHR pcri = {};
		pcri.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pcri.colorAttachmentCount = 1;
		//pcri.pColorAttachmentFormats = vlk.frameCount;

		//this initializes imgui for Vulkan
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = (VkInstance)instance;
		initInfo.PhysicalDevice = vlk.physicalDevice;
		initInfo.Device = vlk.device;
		initInfo.Queue = (VkQueue)gfxQueue;
		initInfo.DescriptorPool = uiCtx.uiDescriptorPool;
		initInfo.MinImageCount = vlk.frameCount;
		initInfo.ImageCount = vlk.frameCount;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.RenderPass = vlk.renderPass;
		initInfo.PipelineRenderingCreateInfo = pcri;

		status = ImGui_ImplVulkan_Init(&initInfo);

		// ImGui should be initialized at this point?
		ImGui::GetIO().DisplaySize = ImVec2(width, height);
		VkCommandBuffer commandBuffer;
		unsigned int currentBuffer;
		vlk.vlkSurface.GetSwapchainCurrentImage(currentBuffer);
		vlk.vlkSurface.GetCommandBuffer(currentBuffer, (void**)&commandBuffer);

		status = ImGui_ImplVulkan_CreateFontsTexture();

		std::cout << "GUI Loaded?";
	}

	void Update_UIContext(entt::registry& registry, entt::entity entity)
	{
		std::cout << "\n\n==== > Updating UI\n\n";

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplVulkan_NewFrame();

		GW::SYSTEM::GWindow& win = registry.get<GW::SYSTEM::GWindow>(registry.group<GW::SYSTEM::GWindow>().front());
		unsigned int width = 0, height = 0;
		win.GetClientWidth(width);
		win.GetClientHeight(height);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(width, height);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		ImGui::NewFrame();

		ImGui::ShowDemoWindow();
	}

	void Destroy_UIContext(entt::registry& registry, entt::entity entity)
	{
		ImGui_ImplVulkan_DestroyFontsTexture();
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplVulkan_Shutdown();
		ImGui::DestroyContext();
	}

	CONNECT_COMPONENT_LOGIC() {
		registry.on_construct<UI::UIData>().connect<Construct_UIContext>();
		registry.on_update<UI::UIData>().connect<Update_UIContext>();
		registry.on_destroy<UI::UIData>().connect<Destroy_UIContext>();
	}

}; // namespace UI