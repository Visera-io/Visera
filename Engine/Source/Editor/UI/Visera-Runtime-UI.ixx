module;
#include <Visera.h>
export module Visera.Editor.UI;

import Visera.Runtime.RHI;
import :ImGui;

export namespace VE { namespace Editor
{

	class UI
	{
		VE_MODULE_MANAGER_CLASS(UI);
	public:
		VE_API Hello() -> StringView { return "Hello Editor"; }

	private:
		VE_API Bootstrap()
		{
			auto* API = Runtime::RHI::GetAPI();
			//[TODO]
			//ImGui = new FImGui(FImGui::CreateInfo
			//{
			//	.Instance		= API->Instance,
			//	//.PhysicalDevice = API->GPU.GetHandle(),
			//	.Device			= API->Device.GetHandle(),
			//	.QueueFamily	= API->Device.GetQueueFamily(RHI::EQueueFamily::Graphics).Index,
			//	.Queue			= API->Device.GetQueueFamily(RHI::EQueueFamily::Graphics).Queues[0],
			//	//.RenderPass;      // Ignored if using dynamic rendering [TODO]
			//	//.DescriptorPool	= RHI::
			//	//.Subpass
			//	.MinImageCount	= UInt32(API->Swapchain.GetImages().size()),
			//	.ImageCount		= UInt32(API->Swapchain.GetImages().size()),
			//	.MSAASamples	= AutoCast(AutoCast(RHI::ESampleRate::X1)),

			//	.PipelineCache	= API->GraphicsPipelineCache.GetHandle(),
			//	//[Debug]
			//	.Allocator		= API->AllocationCallbacks,
			//});
		}

		VE_API Terminate()
		{
			delete ImGui;
		}

	private:
		static inline FImGui* ImGui;
	};

} } // namespace VE::Editor