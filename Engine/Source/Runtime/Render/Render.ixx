module;
#include <Visera>

export module Visera.Engine.Runtime.Render;
export import Visera.Engine.Runtime.Render.RHI;
//export import Visera.Engine.Runtime.Render.Coordinate;

import Visera.Engine.Runtime.Context;
import Visera.Engine.Runtime.Platform;
import Visera.Engine.Core.Log;

namespace VE
{
	class ViseraRuntime;
}

export namespace VE { namespace Runtime
{
	
	class Render
	{
		friend class ViseraRuntime;
	private:
		static inline void
		Tick(SharedPtr<RHI::CommandBuffer> AppDrawCalls)
		{
			if (!RuntimeContext::MainLoop.ShouldStop())
			{
				//Check Window State
				if (Platform::GetWindow().ShouldClose())
				{ return RuntimeContext::MainLoop.Stop(); }

				auto& CurrentFrame = RHI::WaitForSwapchain();
				
				RHI::CommandPool::SubmitInfo Submit
				{
					.Deadlines = {RHI::PipelineStages::ColorAttachmentOutput},
					.CommandBuffers = {AppDrawCalls->GetHandle()},
					.WaitSemaphores = { CurrentFrame.Semaphore_ReadyToRender },
					.SignalSemaphores = { CurrentFrame.Semaphore_ReadyToPresent },
				};
				RHI::GetResetableGraphicsCommandPool().Submit(Submit);
				RHI::PresentSwapchain(CurrentFrame.Semaphore_ReadyToPresent);

				Platform::GetWindow().PollEvents();
			}
		}

		static inline void
		Bootstrap()
		{
			if (!RuntimeContext::Render.IsOffScreenRendering())
			{ Platform::GetWindow(); } // Create Window
			RHI::Bootstrap();
		}

		static inline void
		Terminate()
		{
			RHI::Terminate();
		}

		Render() noexcept = default;
	};
	

} } // namespace VE::Runtime