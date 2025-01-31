module;
#include <Visera.h>
export module Visera.Runtime.Render;
export import Visera.Runtime.RHI;

export namespace VE { namespace Runtime
{
	class ViseraRuntime;

	class Render
	{
		VE_MODULE_MANAGER_CLASS(Render);
		friend class ViseraRuntime;

	private:
		VE_API Bootstrap() -> void { RHI::Bootstrap(); };
		VE_API Tick()	   -> void {  };
		VE_API Terminate() -> void { RHI::Terminate(); };
	};

} } // namespace VE::Runtime