module;
#include <Visera.h>
export module Visera.Runtime.Render.RTC;
import Visera.Runtime.Render.RTC.Embree;
import Visera.Runtime.Render.RTC.AS;
import Visera.Runtime.Render.RTC.Ray;

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
		static inline auto
		Bootstrap() -> void { Embree::Bootstrap(); }
		static inline auto
		Terminate() -> void { Embree::Terminate(); }
	};

}// namespace VE