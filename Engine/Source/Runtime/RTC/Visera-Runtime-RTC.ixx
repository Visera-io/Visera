module;
#include <Visera.h>
export module Visera.Runtime.RTC;

import Visera.Runtime.RTC.Embree;
import Visera.Runtime.RTC.Ray;
import Visera.Runtime.RTC.Geometry;
import Visera.Runtime.RTC.Scene;

import Visera.Core.Signal;

export namespace VE
{

	class RTC
	{
		VE_MODULE_MANAGER_CLASS(RTC);
	public:
		using FRay  = FRay;
		using FMesh = FMesh;
		using FScene = FScene;

		using EBufferType  = Embree::EBufferType;
		using ETopology    = Embree::ETopology;

	//private:
		static inline auto
		Bootstrap() -> void { Embree::Bootstrap(); }
		static inline auto
		Terminate() -> void { Embree::Terminate(); }
	};

	String Text(const FRay& _Ray)
	{ return Text("Ray: {} -> {}", _Ray.GetOrigin(), _Ray.GetDirection()); }

}// namespace VE