module;
#include <Visera.h>

export module Visera.Runtime;
export import Visera.Runtime.Platform;
export import Visera.Runtime.Render;

import Visera.Core.Log;
import Visera.Core.Signal;

VISERA_PUBLIC_MODULE
class Visera;

class ViseraRuntime
{
	friend class Visera;
private:
	static inline Bool
	Tick()
	{	
		try
		{
			Platform::Tick();
			Render::Tick();
		}
		catch(const RuntimeError& Error)
		{
			Log::Error("Visera Runtime Error:\n{}{}", Error.What(), Error.Where());
			throw EngineStopSignal("An unsolved Runtime Error!", VISERA_ENGINE_ERROR);
		}
		return True;
	}

	static inline void
	Bootstrap()
	{
		Platform::Bootstrap();
		Render::Bootstrap();
	}

	static inline void
	Terminate()
	{
		Render::Terminate();
		Platform::Terminate();
	}

	ViseraRuntime() noexcept = default;
};

VISERA_MODULE_END