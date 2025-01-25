module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.World.RTC.Embree:Enums;

export namespace VE { namespace Runtime
{

	VE_ENUM_CLASS(EGeomtryBuffer, RTCBufferType)
	{
		Index				= RTC_BUFFER_TYPE_INDEX,
		Vertex				= RTC_BUFFER_TYPE_VERTEX,
		VertexAttribute		= RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
		Normal				= RTC_BUFFER_TYPE_NORMAL,
		Tangent				= RTC_BUFFER_TYPE_TANGENT,
		NormalDerivative	= RTC_BUFFER_TYPE_NORMAL_DERIVATIVE,

		Grid				= RTC_BUFFER_TYPE_GRID,

		Face				= RTC_BUFFER_TYPE_FACE,
		Level				= RTC_BUFFER_TYPE_LEVEL,
		EdgeCreaseIndex		= RTC_BUFFER_TYPE_EDGE_CREASE_INDEX ,
		EdgeCreaseWeight	= RTC_BUFFER_TYPE_EDGE_CREASE_WEIGHT,
		VertexCreaseIndex	= RTC_BUFFER_TYPE_VERTEX_CREASE_INDEX,
		VertexCreaseWeight  = RTC_BUFFER_TYPE_VERTEX_CREASE_WEIGHT,
		Hole				= RTC_BUFFER_TYPE_HOLE,

		Transform			= RTC_BUFFER_TYPE_TRANSFORM,
	};
	
} } // namespace Visera::Runtime