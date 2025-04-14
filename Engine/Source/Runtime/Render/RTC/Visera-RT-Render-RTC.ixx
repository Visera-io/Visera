module;
#include <Visera.h>
export module Visera.Runtime.Render.RTC;
#define VE_MODULE_NAME "RTC"
import Visera.Runtime.Render.RTC.Embree;
import Visera.Runtime.Render.RTC.AS;
import Visera.Runtime.Render.RTC.Ray;

import Visera.Core.Log;
import Visera.Core.Signal;
import Visera.Core.Media.Model;

export namespace VE
{
	class RTC
	{
		VE_MODULE_MANAGER_CLASS(RTC);
	public:
		using FRay = FRay;
		using FAccelerationStructure = FAccelerationStructure;
		using FSceneNode = FAccelerationStructure::FNode;

		using EBufferType  = Embree::EBufferType;
		using ETopology    = Embree::ETopology;

		static inline auto
		CreateAccelerationStructure() -> SharedPtr<FAccelerationStructure> { return CreateSharedPtr<FAccelerationStructure>(); }

	//private:
		static inline void
		Bootstrap()
		{
			VE_LOG_TRACE("Bootstrapping Embree");
			Embree::Bootstrap();
		}
		static inline void
		Terminate()
		{
			VE_LOG_TRACE("Terminating Embree");
			Embree::Terminate();
		}
	};

}// namespace VE