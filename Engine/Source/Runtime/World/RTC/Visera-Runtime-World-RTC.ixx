module;
#include <Visera.h>
#include <embree4/rtcore.h>
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
		using EBuffer	= EEmbreeBuffer;
		using EType		= EEmbreeType;
		using ETopology = EEmbreeTopology;

		VE_API CreateGeometry(ETopology _Topology)		-> RTCGeometry { return rtcNewGeometry(Embree->GetDevice(), AutoCast(_Topology)); }
		VE_API CommitGeometry(RTCGeometry _Geometry)	-> void { VE_ASSERT(_Geometry != nullptr); rtcCommitGeometry(_Geometry); }
		VE_API DestroyGeometry(RTCGeometry _Geometry)	-> void { if (_Geometry) { rtcReleaseGeometry(_Geometry); } }
		
		VE_API GetAPI() -> const FEmbree* { return Embree; }

	private:
		static inline FEmbree* Embree = nullptr;

		VE_API Bootstrap() -> void { Embree = new FEmbree(); }
		VE_API Terminate() -> void { delete Embree; }
	};

} } // namespace Visera::Runtime