module;
#include <ViseraEngine>

export module Visera.Render;
export import Visera.Render.RHI;
export import Visera.Render.Coordinate;

import Visera.Internal;
import Visera.Platform;
import Visera.Core.Log;

export namespace VE
{
	class Visera;

	class ViseraRender
	{
		friend class Visera;
	private:
		static inline void
		Tick()
		{
			static Bool bContinue = True;
			if (!ViseraInternal::Context.MainLoop.ShouldStop())
			{
				//Window Tick
				if ((bContinue = !ViseraPlatform::GetWindow().ShouldClose()) != True)
				{ return ViseraInternal::Context.MainLoop.Stop("ViseraPlatform - Window"); }

				ViseraPlatform::GetWindow().PollEvents();
			}
		}

		static inline void
		Bootstrap()
		{
			ViseraPlatform::GetWindow();
			RHI::Bootstrap();
		}

		static inline void
		Terminate()
		{
			RHI::Terminate();
		}

		ViseraRender() noexcept = default;
	};
	

} // namespace VE