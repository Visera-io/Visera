module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.World.RTC.Geometry;
import Visera.Runtime.World.RTC.Embree;

import Visera.Core.Signal;

export namespace VE { namespace Runtime
{
	
	class FEmbreeGeometry
	{
	public:
		auto GetHandle() const -> const RTCGeometry { return Handle; }

		FEmbreeGeometry() = delete;
		FEmbreeGeometry(UInt64 _VertexCount, UInt64 _IndexCount);

	private:
		RTCGeometry Handle;
		Float*  Vertices = nullptr;
		UInt32* Indices  = nullptr;
	};

	FEmbreeGeometry::
	FEmbreeGeometry(UInt64 _VertexCount, UInt64 _IndexCount)
	{
		Vertices = (Float*)rtcSetNewGeometryBuffer(
			Handle,
			RTC_BUFFER_TYPE_VERTEX,
			0,
			RTC_FORMAT_FLOAT3,
			3 * sizeof(Float), 3);
	}

} } // namespace Visera::Runtime