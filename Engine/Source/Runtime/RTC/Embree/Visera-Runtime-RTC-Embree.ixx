module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.RTC.Embree;
export import :Common;
export import :Device;

export namespace VE
{

	class Embree
	{
		VE_MODULE_MANAGER_CLASS(Embree);
	public:
		using FGeometry   = RTCGeometry;
		using FScene	  = RTCScene;
		using FRayHit     = RTCRayHit;
		using FRay		  = RTCRay;
		using FHit		  = RTCHit;

		using EBufferType = EEmbreeBufferType;
		using ETopology   = EEmbreeTopology;
		using EFormat	  = EEmbreeFormat;

		enum
		{
			InvalidGeometryID = RTC_INVALID_GEOMETRY_ID,
		};

		static inline auto
		GetDevice() -> const FEmbreeDevice* { return Device; }

	private:
		static inline FEmbreeDevice* Device {nullptr};

	public:
		static inline void
		Bootstrap()
		{
			Device = new FEmbreeDevice();
		}

		static inline void
		Terminate()
		{
			delete Device;
		}
	};

}// namespace VE