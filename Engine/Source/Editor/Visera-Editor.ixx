module;
#include <Visera.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
export module Visera.Editor;
#define VE_MODULE_NAME "Editor"
import Visera.Editor.Widget;

import Visera.Runtime.Platform.Window;
import Visera.Runtime.Render.RHI;
import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.Media;
import Visera.Core.OS.FileSystem;

export namespace VE
{

	class Editor
	{
		VE_MODULE_MANAGER_CLASS(Editor);
	public:
		using FCanvas = FCanvas;

		static inline void
		Display();
		static inline auto
		CreateCanvas(SharedPtr<const IImage> _Image) -> WeakPtr<FCanvas>; //[TODO]: Return FName instead of WeakPtr

	private:
		static inline String LayoutFilePath{FPath{"ViseraEditor.ini"}.ToPlatformString()};
		static inline HashMap<FName, SharedPtr<IWidget>> Widgets;

		static inline SharedPtr<RHI::FDescriptorSetLayout> ImGuiDescriptorSetLayout;

	public:
		class FUIRenderPass : public RHI::FRenderPass
		{
		public:
			FUIRenderPass() : RHI::FRenderPass{ EType::Overlay }
			{
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
			VE_ASSERT(!_EditorCommandBuffer->IsRecording());
	
			_EditorCommandBuffer->StartRecording();

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			_EditorCommandBuffer->ReachRenderPass(EditorRenderPass);
		}

		static void
		EndFrame(SharedPtr<RHI::FGraphicsCommandBuffer> _EditorCommandBuffer)
		{
			VE_ASSERT(_EditorCommandBuffer->IsRecording());
			ImGui::Render();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _EditorCommandBuffer->GetHandle());

			_EditorCommandBuffer->LeaveRenderPass(EditorRenderPass);

			_EditorCommandBuffer->StopRecording();

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
			FileSystem::CreateFileIfNotExists(FPath{LayoutFilePath.c_str()});
			IO.IniFilename =  LayoutFilePath.c_str();
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
					{ VE_LOG_ERROR("ImGUI Vulkan Error Code: {}", UInt32(Err)); }
				},
			};

			ImGui_ImplGlfw_InitForVulkan(Window::GetHandle(), True); // Install callbacks via ImGUI
			
			auto VulkanInstanceHandle = API->GetInstance().GetHandle();
			ImGui_ImplVulkan_Init(&CreateInfo);

			ImGuiDescriptorSetLayout = RHI::CreateDescriptorSetLayout()
				->AddBinding(0, RHI::EDescriptorType::CombinedImageSampler, 1, RHI::EShaderStage::Fragment)
				->Build();
		}

		static void Terminate()
		{
			ImGui::SaveIniSettingsToDisk(LayoutFilePath.c_str());
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();

			Widgets.clear();

			EditorRenderPass.reset();
			ImGuiDescriptorSetLayout.reset();
		}

	private:
		static inline SharedPtr<FUIRenderPass> EditorRenderPass;
		static inline UInt32				   Configurations = 0/*ImGuiConfigFlags_ViewportsEnable |
																   ImGuiConfigFlags_DockingEnable*/;
	};

	WeakPtr<FCanvas> Editor::
	CreateCanvas(SharedPtr<const IImage> _Image)
	{
		auto NewCanvas = FCanvas::Create(RHI::CreateDescriptorSet(ImGuiDescriptorSetLayout));
		Widgets[NewCanvas->GetName()] = NewCanvas;
		NewCanvas->Write(_Image);
		return NewCanvas;
	}

	void Editor::
	Display()
	{
		auto& Frame = RHI::GetCurrentFrame();
		ImGui::Begin("Main Window");
		ImGui::Image(ImTextureID(Frame.GetFinalColorTexture()->GetHandle()), { 1280, 720 });
		ImGui::End();

		for (const auto& [Name, Widget] : Widgets) { Widget->Render(); }
	}
}