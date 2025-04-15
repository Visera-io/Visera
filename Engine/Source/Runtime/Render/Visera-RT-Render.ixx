module;
#include <Visera.h>
export module Visera.Runtime.Render;
export import Visera.Runtime.Render.RHI;
export import Visera.Runtime.Render.RTC;
export import Visera.Runtime.Render.Scene;
export import Visera.Runtime.Render.Shader;
export import Visera.Runtime.Render.URP;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;

export namespace VE
{

	class Render //[TODO]: Remove and add funcs to RHI?
	{
		VE_MODULE_MANAGER_CLASS(Render);
	public:
		static void inline Bootstrap();
		static void inline Terminate();
	};


	void Render::
	Bootstrap()
	{
	}

	void Render::
	Terminate()
	{
		
	}

} // namespace VE