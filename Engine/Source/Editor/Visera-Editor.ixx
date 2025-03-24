module;
#include <Visera.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
export module Visera.Editor;
export import Visera.Editor.Widget;

import Visera.Runtime.Window;
import Visera.Runtime.RHI;
import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.OS.FileSystem;

export namespace VE
{
	class Editor
	{
		VE_MODULE_MANAGER_CLASS(Editor);
	public:
		VE_API CreateWindow() -> void { ImGui::ShowDemoWindow(); }
		static inline void
		RenderWidgets() { for (const auto& [Name, Widget] : Widgets) {Widget->Render(); } }

		template<typename T> static auto
		CreateWidget(FName _Name) -> SharedPtr<T>;

	private:
		static inline FPath LayoutFilePath{VISERA_APP_CACHE_DIR"/Editor.ini"};
		static inline HashMap<FName, SharedPtr<IWidget>> Widgets;

	public:
		class FUIRenderPass : public RHI::FRenderPass
		{
		public:
			FUIRenderPass() : RHI::FRenderPass{ ERenderPassType::Customized }
			{
				Layout.AddColorAttachment(
				{
					.Layout			= RHI::EImageLayout::ColorAttachment,
					.Format			= RHI::EFormat::U32_Normalized_R8_G8_B8_A8,
					.SampleRate		= RHI::ESampleRate::X1,
					.ViewType		= RHI::EImageViewType::Image2D,
					.LoadOp			= RHI::EAttachmentIO::I_Clear,
					.StoreOp		= RHI::EAttachmentIO::O_Store,
					.InitialLayout	= RHI::EImageLayout::ColorAttachment,
					.FinalLayout	= RHI::EImageLayout::ColorAttachment,
				});
				Subpasses.emplace_back(FSubpass
				{ 
					.ColorImageReferences = {0},
					.SrcStage		= RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
					.SrcStageAccess = RHI::EAccess::W_ColorAttachment,
					.DstStage		= RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
					.DstStageAccess = RHI::EAccess::W_ColorAttachment,
					.bExternalSubpass = True
				});
			}
		};

		static void
		BeginFrame(SharedPtr<RHI::FGraphicsCommandBuffer> _EditorCommandBuffer)
		{
			VE_ASSERT(_EditorCommandBuffer->IsRecording());

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			_EditorCommandBuffer->ReachRenderPass(EditorRenderPass);
		}

		static void
		EndFrame(SharedPtr<RHI::FGraphicsCommandBuffer> _EditorCommandBuffer)
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
			auto& IO = ImGui::GetIO();

			// Configuration
			FileSystem::CreateFileIfNotExists(LayoutFilePath);
			IO.IniFilename =  LayoutFilePath.ToPlatformString().c_str();
			IO.ConfigFlags |= Configurations;

			ImFont* EditorFont = IO.Fonts->AddFontFromFileTTF(VISERA_ENGINE_FONTS_DIR"/HelveticaNeueMedium.ttf", 14);

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
				.DescriptorPool	= RHI::GetGlobalDescriptorPool()->GetHandle(),
				.RenderPass		= EditorRenderPass->GetHandle(), // Ignored if using dynamic rendering
				
				.MinImageCount	= UInt32(RHI::GetSwapchainFrameCount()),
				.ImageCount		= UInt32(RHI::GetSwapchainFrameCount()),
				.MSAASamples	= VK_SAMPLE_COUNT_1_BIT,

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
		}

		static void Terminate()
		{
			EditorRenderPass.reset();

			ImGui::SaveIniSettingsToDisk(LayoutFilePath.ToPlatformString().c_str());
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		}

	private:
		static inline SharedPtr<FUIRenderPass> EditorRenderPass;
		static inline UInt32				   Configurations = 0/*ImGuiConfigFlags_ViewportsEnable |
																   ImGuiConfigFlags_DockingEnable*/;
	};

	template<typename T> SharedPtr<T> Editor::
	CreateWidget(FName _Name)
	{
		if (Widgets.count(_Name))
		{ throw SRuntimeError("Widget already exists"); }

		auto NewWidget = CreateSharedPtr<T>();
		Widgets[_Name] = NewWidget;

		return NewWidget;
	}

}