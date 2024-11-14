module;
#include <Visera>

export module Visera.Engine.Runtime;
export import Visera.Engine.Runtime.Platform;
export import Visera.Engine.Runtime.Render;
import Visera.Engine.Runtime.Context;

import Visera.Engine.Core.Log;
import Visera.Engine.Internal;

import Visera.App;

export namespace VE
{
	class Visera;

	class ViseraRuntime
	{
		friend class Visera;
	private:
		static inline Bool
		Tick(ViseraApp* App)
		{
			if (!Runtime::RuntimeContext::MainLoop.ShouldStop())
			{
				Runtime::Platform::Tick();

				Runtime::Render::Tick(App->RenderTick());

				return True;
			}
			return False;
		}

		static inline void
		Bootstrap()
		{
			Runtime::RuntimeContext::Bootstrap();
			Runtime::Platform::Bootstrap();
			Runtime::Render::Bootstrap();
		}

		static inline void
		Terminate()
		{
			Runtime::Render::Terminate();
			Runtime::Platform::Terminate();
			Runtime::RuntimeContext::Terminate();
		}

		ViseraRuntime() noexcept = default;
	};

} // namespace VE