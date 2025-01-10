module;
#include <Visera.h>

export module Visera.Runtime;
//Runtime Modules
export import Visera.Runtime.Window;
export import Visera.Runtime.RHI;
export import Visera.Runtime.UI;
export import Visera.Runtime.Object;//[TODO]: Remove

import Visera.Core.Log;
import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	inline Bool
	Tick()
	{	
		try
		{
			//Platform::Tick();
			//Render::Tick();
		}
		catch(const RuntimeError& Error)
		{
			Log::Error("Visera Runtime Error:\n{}{}", Error.What(), Error.Where());
			throw EngineStopSignal("An unsolved Runtime Error!", VISERA_ENGINE_ERROR);
		}
		return True;
	}

	inline void
	Bootstrap()
	{
		//Platform::Bootstrap();
		//Render::Bootstrap();
	}

	inline void
	Terminate()
	{
		//Render::Terminate();
		//Platform::Terminate();
	}

	inline Array<std::function<void()>> Drawcalls;

	inline void SubmitDrawCall(std::function<void()> _DrawCall)
	{ Drawcalls.push_back(_DrawCall); }

	inline void
	RenderTick()
	{
		//Check Window State
		Window::PollEvents();

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
			throw RuntimeError("Not support Swapchain recreation right now.");
		}
	}

} } // namespace VE::Runtime