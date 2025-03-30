module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.Render.Scene.Primitive:Mesh;
import :Interface;

import Visera.Runtime.Render.RTC.Embree;

export namespace VE
{

    // class FMesh : public IPrimitive
	// {
	// public:
	// 	enum class EFaceWinding{ Clockwise, CounterClockwise };

	// 	static inline auto
	// 	CreatePlane(const Vector3F& _LeftTop, const Vector3F& _RightTop, const Vector3F& _RightBottom, const Vector3F& _LeftBottom, EFaceWinding _FaceWinding = EFaceWinding::Clockwise) -> SharedPtr<FMesh>;

	// 	FMesh() = delete;
	// 	FMesh(SharedPtr<const FModel> _Model);
    // 	FMesh(const FCreateInfo& _CreateInfo) : IPrimitive{_CreateInfo}{}

	// private:
	// 	EFaceWinding FaceWinding;
	// };

	// SharedPtr<FMesh> FMesh::
	// CreatePlane(const Vector3F& _LeftTop,
	// 			const Vector3F& _RightTop,
	// 	        const Vector3F& _RightBottom,
	// 			const Vector3F& _LeftBottom,
	// 	        EFaceWinding    _FaceWinding/* = EFaceWinding::Clockwise*/)
	// {
	// 	Segment<Vector3F,4> Vertices
	// 	{
	// 		_LeftTop,
	// 		_RightTop,
	// 		_RightBottom,
	// 		_LeftBottom,
	// 	};
	// 	Segment<UInt32, 6> Indices{0,1,2,0,2,3};

	// 	if (EFaceWinding::CounterClockwise == _FaceWinding)
	// 	{
	// 		std::swap(Indices[0], Indices[2]);
	// 		std::swap(Indices[3], Indices[5]);
	// 	}

	// 	auto Plane = CreateSharedPtr<FMesh>(FCreateInfo
	// 	{
	// 		.Topology	 = Embree::ETopology::Triangle,
	// 		.VertexCount = 4,
	// 		.Vertices	 = Vertices.data()->data(),
	// 		.IndexCount  = 6,
	// 		.Indices	 = Indices.data(),
	// 	});
	// 	Plane->FaceWinding = _FaceWinding;

	// 	return Plane; //[TODO]: Add a new FCreateInfo for FMesh.
	// }

	// FMesh::
	// FMesh(SharedPtr<const FModel> _Model)
	// 	:IPrimitive{FCreateInfo
	// 	{
	// 		.Topology    = EEmbreeTopology::Triangle,
	// 		.VertexCount = _Model->GetVertexCount(),
	// 		.Vertices	 = _Model->GetVertexData(),
	// 		.IndexCount	 = _Model->GetIndexCount(),
	// 		.Indices	 = _Model->GetIndexData(),
	// 	}}
	// {

	// }

}// namespace VE