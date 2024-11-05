module;
#include <ViseraEngine>

export module Visera.Runtime.Render;
export import Visera.Runtime.Render.RHI;
//export import Visera.Runtime.Render.Coordinate;

import Visera.Runtime.Context;
import Visera.Runtime.Platform;
import Visera.Core.Log;

export namespace VE
{
	class ViseraRuntime;

	class RenderRuntime
	{
		friend class ViseraRuntime;
	private:
		static inline void
		Tick()
		{
			static Bool bContinue = True;
			if (!RuntimeContext::MainLoop.ShouldStop())
			{
				//Window Tick
				if ((bContinue = !PlatformRuntime::GetWindow().ShouldClose()) != True)
				{ return RuntimeContext::MainLoop.Stop("ViseraPlatform - Window"); }

				PlatformRuntime::GetWindow().PollEvents();
			}
		}

		static inline void
		Bootstrap()
		{
			PlatformRuntime::GetWindow();
			RHI::Layer::Bootstrap();
		}

		static inline void
		Terminate()
		{
			RHI::Layer::Terminate();
		}

		RenderRuntime() noexcept = default;
	};
	

} // namespace VE