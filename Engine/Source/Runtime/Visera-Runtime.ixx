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

			return True;
		}
		catch(const EngineStopSignal& Signal)//[FIXME]: Runtime Terminate Signal
		{
			return False;
		}
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