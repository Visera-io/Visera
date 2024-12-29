module;
#include <Visera.h>

export module Visera.Runtime.Render;
export import Visera.Runtime.Render.RHI;
//export import Visera.Runtime.Render.Coordinate;

import Visera.Runtime.Context;
import Visera.Runtime.Platform;
import Visera.Core.Log;

VISERA_PUBLIC_MODULE

class ViseraRuntime;
	
class Render
{
	friend class ViseraRuntime;
public:
	static inline void SubmitDrawCall(std::function<void()> _DrawCall)
	{ Drawcalls.push_back(_DrawCall); }

private:
	static inline Array<std::function<void()>> Drawcalls;

private:
	static inline void
	Tick()
	{
		if (RuntimeContext::MainLoop.ShouldStop()) return;

		//Check Window State
		if  (Platform::GetWindow().ShouldClose()) return RuntimeContext::MainLoop.Stop();
		else Platform::GetWindow().PollEvents();

		try
		{
			RHI::WaitForCurrentFrame();
			{
				auto& CurrentFrame = RHI::GetCurrentFrame();
				VE_ASSERT(Drawcalls.size() > 0);
				//[TODO]: CommandList is submitted via App;
				auto& DrawCall = Drawcalls.front();
				DrawCall();
				Drawcalls.clear();
				std::exit(VISERA_APP_ERROR); // TEST

				Array<RHI::CommandPool::SubmitInfo> SubmitInfos;

				for (const auto& [Name,CommandContext] : CurrentFrame.CommandContexts)
				{
					SubmitInfos.emplace_back(RHI::CommandPool::SubmitInfo
					{
						.Deadlines = {AutoCast(RHI::EPipelineStage::ColorAttachmentOutput)},
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
		catch (const RHI::SwapchainRecreateSignal&)
		{
			Log::Fatal("Not support Swapchain recreation right now.");
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

VISERA_MODULE_END