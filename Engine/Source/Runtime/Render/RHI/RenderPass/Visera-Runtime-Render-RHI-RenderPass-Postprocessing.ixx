module;
#include <Visera.h>
export module Visera.Runtime.Render.RHI.RenderPass:Postprocessing;

import Visera.Runtime.Render.RHI.Vulkan;

export namespace VE { namespace Runtime
{

	class FPostprocessingPass : FVulkanRenderPass
	{
	public:
		virtual void Render(SharedPtr<FVulkanCommandBuffer> _CommandBuffer) override;

	private:
		//class FFullScreenTriangleSubpass : public FSubpass{};
	};

	void FPostprocessingPass::
	Render(SharedPtr<FVulkanCommandBuffer> _CommandBuffer)
	{
		VE_ASSERT(_CommandBuffer->IsRecording());


	}
	
} } // namespace VE::Runtime