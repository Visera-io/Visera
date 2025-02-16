module;
#include <Visera.h>
export module Visera.Runtime;
//Runtime Modules [TODO]: remove exports and export Runtime in Visera.ixx
export import Visera.Runtime.Window;
export import Visera.Runtime.RHI;
export import Visera.Runtime.Render;
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
				RHI::Tick();
				if (Window::ShouldClose()) { throw SEngineStop("Window has been closed."); }
			}
			catch(const SRuntimeError& RuntimeError)
			{ Log::Fatal(Text("Visera Engine Internal Runtime Error:\n{}{}", RuntimeError.What(), RuntimeError.Where())); }
			return True;
		}

		VE_API Bootstrap() -> void
		{
			World::Bootstrap();
			Window::Bootstrap();
			RHI::Bootstrap();
			Render::Bootstrap();
		}

		VE_API Terminate() -> void
		{
			Render::Terminate();
			RHI::Terminate();
			Window::Terminate();
			World::Terminate();
		}
	};
	
} } // namespace VE::Runtime