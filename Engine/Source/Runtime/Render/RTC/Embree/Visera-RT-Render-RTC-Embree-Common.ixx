module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.Render.RTC.Embree:Common;

export namespace VE
{

	VE_ENUM_CLASS(EEmbreeBufferType, RTCBufferType)
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

	VE_ENUM_CLASS(EEmbreeFormat, RTCFormat)
	{
		Float				= RTC_FORMAT_FLOAT,
		UInt32				= RTC_FORMAT_UINT,

		Index				= RTC_FORMAT_UINT,
		TriangleIndices		= RTC_FORMAT_UINT3,
		QuadIndices			= RTC_FORMAT_UINT4,

		Vector2F			= RTC_FORMAT_FLOAT2,
		Vector3F			= RTC_FORMAT_FLOAT3,
		Vector4F			= RTC_FORMAT_FLOAT4,

		Matrix2x2F			= RTC_FORMAT_FLOAT2X2_COLUMN_MAJOR,
		Matrix3x3F			= RTC_FORMAT_FLOAT3X3_COLUMN_MAJOR,
		Matrix4x4F			= RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR,
	};

	VE_ENUM_CLASS(EEmbreeTopology, RTCGeometryType)
	{
		Triangle = RTC_GEOMETRY_TYPE_TRIANGLE,		// triangle mesh
		Quad     = RTC_GEOMETRY_TYPE_QUAD,			// quad (triangle pair) mesh
		Grid     = RTC_GEOMETRY_TYPE_GRID,			// grid mesh

		None     = ~0U,
	};
	
}// namespace VE