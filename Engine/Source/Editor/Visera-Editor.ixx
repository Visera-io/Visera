module;
#include <Visera.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
export module Visera.Editor;
import Visera.Runtime.Window;
import Visera.Runtime.RHI;
import Visera.Core.Log;

export namespace VE
{
	class Editor
	{
		VE_MODULE_MANAGER_CLASS(Editor);
	public:
		VE_API CreateWindow() -> void { ImGui::ShowDemoWindow(); }

	private:
		class FUIRenderPass : public RHI::FRenderPass
		{
		public:
			FUIRenderPass() : RHI::FRenderPass{ ERenderPassType::Customized }
			{
				Layout.AddColorAttachment(
				{
					.Layout			= EVulkanImageLayout::ColorAttachment,
					.Format			= EVulkanFormat::U32_Normalized_R8_G8_B8_A8,
					.SampleRate		= EVulkanSampleRate::X1,
					.ViewType		= EVulkanImageViewType::Image2D,
					.LoadOp			= EVulkanAttachmentIO::I_Keep,
					.StoreOp		= EVulkanAttachmentIO::O_Store,
					.InitialLayout	= EVulkanImageLayout::Undefined,//[FIXME]
					.FinalLayout	= EVulkanImageLayout::TransferSource,
				});
				Subpasses.emplace_back(FSubpass
				{ 
					.ColorImageReferences = {0},
					.SrcStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
					.SrcStageAccess = RHI::EAccessibility::None,
					.DstStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
					.DstStageAccess = RHI::EAccessibility::W_ColorAttachment,
					.bExternalSubpass = True
				});
			}
		};

		static void BeginFrame(SharedPtr<RHI::FGraphicsCommandBuffer> _EditorCommandBuffer)
		{
			VE_ASSERT(_EditorCommandBuffer->IsRecording());
			//[TODO]: Blit Image

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			_EditorCommandBuffer->ReachRenderPass(EditorRenderPass);
		}

		static void EndFrame(SharedPtr<RHI::FGraphicsCommandBuffer> _EditorCommandBuffer)
		{
			ImGui::Render();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _EditorCommandBuffer->GetHandle());

			_EditorCommandBuffer->LeaveRenderPass(EditorRenderPass);

			////At the end of your render loop, generally after rendering your main viewport but before presenting/swapping it:
			//if (Configurations & ImGuiConfigFlags_ViewportsEnable)
			//{
			//	/*ImGui::UpdatePlatformWindows();
			//	ImGui::RenderPlatformWindowsDefault();*/
			//}
		}

		static void Bootstrap()
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

		static void Terminate()
		{
			EditorRenderPass.reset();
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		}

	private:
		static inline SharedPtr<FUIRenderPass> EditorRenderPass;
		static inline UInt32				   Configurations = 0/*ImGuiConfigFlags_ViewportsEnable |
																   ImGuiConfigFlags_DockingEnable*/;
	};

}