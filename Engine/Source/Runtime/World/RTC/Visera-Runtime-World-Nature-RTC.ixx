module;
#include <Visera.h>
export module Visera.Runtime.World.RTC;
import  Visera.Runtime.World.RTC.Embree;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	class World;

	class RTC
	{
		VE_MODULE_MANAGER_CLASS(RTC);
		friend class World;
	public:
		VE_API GetAPI() -> const FEmbree* { return Embree; }

	private:
		static inline FEmbree* Embree = nullptr;

		VE_API Bootstrap() -> void { Embree = new FEmbree(); }
		VE_API Terminate() -> void { delete Embree; }
	};

} } // namespace Visera::Runtime