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
			if (!RuntimeContext::MainLoop.ShouldStop())
			{
				//Check Window State
				static const auto& Window = Platform::Window::GetInstance();
				if (Window.ShouldClose())
				{ return RuntimeContext::MainLoop.Stop(); }

				Window.PollEvents();
			}
		}

		static inline void
		Bootstrap()
		{
			if (!RuntimeContext::Render.IsOffScreenRendering())
			{ Platform::Window::GetInstance(); }
			Render::RHI::Bootstrap();
		}

		static inline void
		Terminate()
		{
			Render::RHI::Terminate();
		}

		RenderRuntime() noexcept = default;
	};
	

} // namespace VE