module;
#include <Visera.h>

export module Visera.Runtime;
//Runtime Modules
export import Visera.Runtime.Render;
export import Visera.Runtime.Window;
export import Visera.Runtime.World;

import Visera.Core.Log;
import Visera.Core.Signal;

export namespace VE { namespace Runtime
{

	class ViseraRuntime
	{
		VE_MODULE_MANAGER_CLASS(ViseraRuntime);
	public:
		VE_API Tick() -> Bool
		{	
			try
			{
				World::Tick();
				Window::Tick();
				Render::Tick();
				if (Window::ShouldClose()) { throw SEngineStop("Window has been closed."); }
			}
			catch(const SRuntimeError& RuntimeError)
			{ Log::Fatal(Text("Visera Runtime Error:\n{}{}", RuntimeError.What(), RuntimeError.Where())); }
			return True;
		}

		VE_API Bootstrap() -> void
		{
			World::Bootstrap();
			Window::Bootstrap();
			Render::Bootstrap();
		}

		VE_API Terminate() -> void
		{
			Render::Terminate();
			Window::Terminate();
			World::Terminate();
		}

		static inline Array<std::function<void()>> Drawcalls;

		static inline void SubmitDrawCall(std::function<void()> _DrawCall)
		{ Drawcalls.push_back(_DrawCall); }

		VE_API RenderTick() -> void
		{
			try
			{
				VE_WIP;
				//RHI::WaitForCurrentFrame();
				//{
				//	auto& CurrentFrame = RHI::GetCurrentFrame();
				//	VE_ASSERT(Drawcalls.size() > 0);
				//	//[TODO]: CommandList is submitted via App;
				//	auto& DrawCall = Drawcalls.front();
				//	DrawCall();
				//	Drawcalls.clear();
				//	std::exit(VISERA_APP_ERROR); // TEST

				//	Array<RHI::FCommandPool::SubmitInfo> SubmitInfos;

				//	for (const auto& [Name,CommandContext] : CurrentFrame.CommandContexts)
				//	{
				//		SubmitInfos.emplace_back(RHI::FCommandPool::SubmitInfo
				//		{
				//			.Deadlines = {AutoCast(RHI::EPipelineStage::ColorAttachmentOutput)},
				//			.CommandBuffers = {CommandContext->Commands->GetHandle()},
				//			.WaitSemaphores = {CurrentFrame.Semaphore_ReadyToRender},//[FIXME] Temp
				//			.SignalSemaphores = { /*TEST*/ CurrentFrame.Semaphore_ReadyToPresent },
				//			.Fence = CurrentFrame.Fence_Rendering
				//		});
				//	}
				//	VE_ASSERT(SubmitInfos.size() == 1, "TESTING"); //Visera Render is controled by a singlton cmd
				//	RHI::ResetableGraphicsCommandPool.Submit(SubmitInfos[0]);
				//}
				//RHI::PresentCurrentFrame();
			}
			catch (const RHI::SwapchainRecreateSignal&)
			{
				throw SRuntimeError("Not support Swapchain recreation right now.");
			}
		}
	};

	

} } // namespace VE::Runtime