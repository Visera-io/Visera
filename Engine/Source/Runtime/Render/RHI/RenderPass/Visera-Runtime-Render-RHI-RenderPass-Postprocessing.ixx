module;
#include <Visera.h>
export module Visera.Runtime.Render.RHI.RenderPass:Postprocessing;

import Visera.Runtime.Render.RHI.Vulkan;

export namespace VE { namespace Runtime
{

	class FPostprocessingPass : FVulkanRenderPass
	{
	public:
		virtual void Render(SharedPtr<FVulkanCommandPool::FVulkanCommandBuffer> _CommandBuffer) override;

	private:
		class FFullScreenTriangleSubpass : public Subpass{};
	};

	void FPostprocessingPass::
	Render(SharedPtr<FVulkanCommandPool::FVulkanCommandBuffer> _CommandBuffer)
	{
		VE_ASSERT(_CommandBuffer->IsRecording());


	}
	
} } // namespace VE::Runtime