module;
#include <Visera.h>

export module Visera.Runtime;
export import Visera.Runtime.Platform;
export import Visera.Runtime.Render;
import Visera.Runtime.Context;

import Visera.Core.Log;
import Visera.Internal;

import Visera.App;

VISERA_PUBLIC_MODULE
	class Visera;

	class ViseraRuntime
	{
		friend class Visera;
	private:
		static inline Bool
		Tick(ViseraApp* App)
		{	
			if (!RuntimeContext::MainLoop.ShouldStop())
			{
				Platform::Tick();

				static const auto AppRenderTick = std::bind(&ViseraApp::RenderTick, App);
				Render::Tick(AppRenderTick);

				return True;
			}
			return False;
		}

		static inline void
		Bootstrap()
		{
			RuntimeContext::Bootstrap();
			Platform::Bootstrap();
			Render::Bootstrap();
		}

		static inline void
		Terminate()
		{
			Render::Terminate();
			Platform::Terminate();
			RuntimeContext::Terminate();
		}

		ViseraRuntime() noexcept = default;
	};

VISERA_MODULE_END