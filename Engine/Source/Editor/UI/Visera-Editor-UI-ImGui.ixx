module;
#include <Visera.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
export module Visera.Editor.UI:ImGui;
import :UIRenderPass;

import Visera.Core.Signal;
import Visera.Core.Log;
import Visera.Runtime.Window;
import Visera.Runtime.RHI;

export namespace VE { namespace Editor
{
	using namespace VE::Runtime;

	class UI;

	class FImGui
	{
		friend class UI;
	public:
		enum class EStatus : UInt8 {Idle, InFrame};
		void BeginFrame()
		{
			VE_ASSERT(Status != EStatus::InFrame);

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			//[FIXME]: Testing
			auto Cmd = RHI::GetCurrentFrame().GetGraphicsCommandBuffer();
			Cmd->StartRecording();
			Cmd->ReachRenderPass(EditorRenderPass);

			Status = EStatus::InFrame;
		}

		void EndFrame(SharedPtr<RHI::FGraphicsCommandBuffer> _GraphicsCommandBuffer)
		{
			VE_ASSERT(Status == EStatus::InFrame);

			ImGui::Render();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),_GraphicsCommandBuffer->GetHandle());

			//[FIXME]: Testing
			auto Cmd = RHI::GetCurrentFrame().GetGraphicsCommandBuffer();
			Cmd->LeaveRenderPass(EditorRenderPass);
			Cmd->StopRecording();

			////At the end of your render loop, generally after rendering your main viewport but before presenting/swapping it:
			//if (Configurations & ImGuiConfigFlags_ViewportsEnable)
			//{
			//	/*ImGui::UpdatePlatformWindows();
			//	ImGui::RenderPlatformWindowsDefault();*/
			//}
			Status = EStatus::Idle;
		}

		void ShowDemoWindow() const { ImGui::ShowDemoWindow(); }

	private:
		EStatus					 Status{ EStatus::Idle };
		SharedPtr<FUIRenderPass> EditorRenderPass;
		UInt32					 Configurations = 0/*ImGuiConfigFlags_ViewportsEnable |
												  ImGuiConfigFlags_DockingEnable*/;

		FImGui()
		{
			EditorRenderPass = RHI::CreateRenderPass<FUIRenderPass>();

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			auto& io = ImGui::GetIO();

			// Configuration
			io.IniFilename =  VISERA_APP_CACHE_DIR"/Editor.layout";
			io.ConfigFlags |= Configurations;

			ImGui::StyleColorsDark();
			//ImGui::StyleColorsLight();

			auto* API = RHI::GetAPI();
			auto& QueueFamily = API->GetDevice().GetQueueFamily(RHI::EQueueFamily::Graphics);

			ImGui_ImplVulkan_InitInfo CreateInfo
			{
				.Instance		= API->GetInstance().GetHandle(),
				.PhysicalDevice = API->GetGPU().GetHandle(),
				.Device			= API->GetDevice().GetHandle(),
				.QueueFamily	= QueueFamily.Index,
				.Queue			= QueueFamily.Queues[0],
				.DescriptorPool	= RHI::GetGlobalDescriptorPool().GetHandle(),
				.RenderPass		= EditorRenderPass->GetHandle(), // Ignored if using dynamic rendering [TODO]
				
				.MinImageCount	= UInt32(RHI::GetSwapchainFrameCount()),
				.ImageCount		= UInt32(RHI::GetSwapchainFrameCount()),
				.MSAASamples	= AutoCast(RHI::ESampleRate::X1),

				.PipelineCache	= API->GetGraphicsPipelineCache().GetHandle(),
				.Subpass		= 0,

				.DescriptorPoolSize = 0,

				.Allocator		= API->GetAllocationCallbacks(),
				.CheckVkResultFn = [](VkResult Err)
				{
					if (Err != VK_SUCCESS)
					{ Log::Error("ImGUI Vulkan Error Code: {}", UInt32(Err)); }
				},
			};

			ImGui_ImplGlfw_InitForVulkan(Window::GetHandle(), True); // Install callbacks via ImGUI
			
			auto VulkanInstanceHandle = API->GetInstance().GetHandle();
			ImGui_ImplVulkan_Init(&CreateInfo);

			//[TODO]: Load Font
		}
	
		~FImGui()
		{
			EditorRenderPass.reset();
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		}
	};

} } // namespace VE::Editor