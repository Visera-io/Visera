module;
#include <ViseraEngine>

export module Visera.Platform;
export import Visera.Platform.Window;

import Visera.Internal;
import Visera.Core.Log;

export namespace VE
{
	class Visera;

	class ViseraPlatform
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
				if ((bContinue = Window::Tick()) != True)
				{ return ViseraInternal::Context.MainLoop.Stop("ViseraPlatform::Window"); }
			}
		}

		static inline void
		Bootstrap()
		{
			Window::Bootstrap();
		}

		static inline void
		Terminate()
		{
			Window::Terminate();
		}

		ViseraPlatform() noexcept = default;

	private:
		
	};
	

} // namespace VE