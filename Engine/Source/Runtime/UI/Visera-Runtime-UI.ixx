module;
#include <Visera.h>

export module Visera.Runtime.UI;
import Visera.Runtime.RHI;
import Visera.Runtime.UI.ImGui;

export namespace VE { namespace Runtime
{
class UI
{
public:

private:
	UI() noexcept = default;
	static void
	Bootstrap()
	{
		auto* API = RHI::GetAPI();
		ImGui::Bootstrap(ImGui::CreateInfo
			{
				.Instance		= API->Instance,
				//.PhysicalDevice = API->GPU.GetHandle(),
				.Device			= API->Device.GetHandle(),
				.QueueFamily	= API->Device.GetQueueFamily(RHI::EQueueFamily::Graphics).Index,
				.Queue			= API->Device.GetQueueFamily(RHI::EQueueFamily::Graphics).Queues[0],
				//.RenderPass;      // Ignored if using dynamic rendering [TODO]
				//.DescriptorPool	= RHI::
				//.Subpass
				.MinImageCount	= UInt32(API->Swapchain.GetImages().size()),
				.ImageCount		= UInt32(API->Swapchain.GetImages().size()),
				.MSAASamples	= AutoCast(AutoCast(RHI::ESampleRate::X1)),

				.PipelineCache	= API->GraphicsPipelineCache.GetHandle(),
				//[Debug]
				.Allocator		= API->AllocationCallbacks,
			});
	}
	static void
	Terminate()
	{
		ImGui::Terminate();
	}
};

} } // namespace VE::Runtime