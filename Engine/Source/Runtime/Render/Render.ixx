module;
#include <Visera.h>

export module Visera.Runtime.Render;
export import Visera.Runtime.Render.RHI;
//export import Visera.Runtime.Render.Coordinate;

import Visera.Runtime.Context;
import Visera.Runtime.Platform;
import Visera.Core.Log;

namespace VE
{
	class ViseraRuntime;
}

export namespace VE
{
	
	class Render
	{
		friend class ViseraRuntime;
	private:
		static inline void
		Tick(const std::function<void()>& AppRenderTick)
		{
			if (!RuntimeContext::MainLoop.ShouldStop())
			{
				//Check Window State
				if  (Platform::GetWindow().ShouldClose()) return RuntimeContext::MainLoop.Stop();
				else Platform::GetWindow().PollEvents();

				try
				{
					RHI::WaitForCurrentFrame();
					{
						auto& CurrentFrame = RHI::GetCurrentFrame();
						AppRenderTick();

						Array<RHI::CommandPool::SubmitInfo> SubmitInfos;

						for (const auto& [Name,CommandContext] : CurrentFrame.CommandContexts)
						{
							SubmitInfos.emplace_back(RHI::CommandPool::SubmitInfo
							{
								.Deadlines = {RHI::PipelineStages::ColorAttachmentOutput},
								.CommandBuffers = {CommandContext->Commands->GetHandle()},
								.WaitSemaphores = {CurrentFrame.Semaphore_ReadyToRender},//[FIXME] Temp
								.SignalSemaphores = { /*TEST*/ CurrentFrame.Semaphore_ReadyToPresent },
								.Fence = CurrentFrame.Fence_Rendering
							});
						}
						VE_ASSERT(SubmitInfos.size() == 1, "TESTING"); //Visera Render is controled by a singlton cmd
						RHI::ResetableGraphicsCommandPool.Submit(SubmitInfos[0]);
					}
					RHI::PresentCurrentFrame();
				}
				catch (const RHI::Swapchain::RecreateSignal&)
				{
					Log::Fatal("Not support Swapchain recreation right now.");
				}
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
	

} // namespace VE