module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.World.Geometry.Mesh;

import Visera.Runtime.World.RTC;
import Visera.Core.Signal;
import Visera.Core.Math;
import Visera.Core.System.Memory;

export namespace VE { namespace Runtime
{
	
	class FMesh
	{
	public:
		using ETopology = RTC::ETopology;
		
		auto GetTopology()	const -> ETopology { return Topology; }
		auto GetHandle()	const -> const RTCGeometry const { return Handle; }

		struct FCreateInfo
		{
			ETopology	  Topology  = ETopology::None;
			UInt64		  VertexCount = 0;
			const Float*  Vertices    = nullptr;
			UInt64		  IndexCount  = 0;
			const UInt32* Indices     = nullptr;
		};
		FMesh() = delete;
		FMesh(const FCreateInfo& _CreateInfo);
		~FMesh();

	protected:
		RTCGeometry Handle		= nullptr;
		ETopology	Topology    = ETopology::None;
		Float*		Vertices    = nullptr;
		UInt64		VertexCount = 0;
		UInt32*		Indices     = nullptr;
		UInt64		IndexCount  = 0;
	};

	FMesh::
	FMesh(const FCreateInfo& _CreateInfo):
		Topology{_CreateInfo.Topology},
		VertexCount{_CreateInfo.VertexCount},
		IndexCount{_CreateInfo.IndexCount}
	{
		Handle = RTC::CreateGeometry(Topology);
		if (!Handle) { throw SRuntimeError("Failed to create the FMesh!"); }

		switch (Topology)
		{
		case ETopology::Triangle:
		{
			VE_ASSERT(VertexCount % 3 == 0 && IndexCount % 3 == 0);

			Vertices = (Float*)rtcSetNewGeometryBuffer(
						Handle,
						AutoCast(RTC::EBuffer::Vertex),
						0,
						AutoCast(RTC::EType::Vector3F),
						sizeof(Vector3F),
						VertexCount / 3);
			if (Vertices)
			{
				Memory::Memcpy(Vertices, _CreateInfo.Vertices, VertexCount * sizeof(Float));
			}
			else { throw SRuntimeError("Failed to allocate Embree Geometry Vertex Buffer!"); }

			Indices  = (UInt32*)rtcSetNewGeometryBuffer(
						Handle,
						AutoCast(RTC::EBuffer::Index),
						0,
						AutoCast(RTC::EType::TriangleIndices),
						sizeof(UInt32) * 3,
						IndexCount / 3);
			if (Indices)
			{
				Memory::Memcpy(Indices, _CreateInfo.Indices, IndexCount * sizeof(UInt32));
			}
			else { throw SRuntimeError("Failed to allocate Embree Geometry Index Buffer!"); }

			break;
		}
		case ETopology::Quad:
		{
			VE_WIP;
			break;
		}
		case ETopology::None:
		{
			throw SRuntimeError("You MUST assign the topology when creating a FMesh!");
			break;
		}
		default:
			throw SRuntimeError("Unsupported Mesh Topology({})!", UInt32(Topology));
		}
	}

	FMesh::~FMesh()
	{
		RTC::DestroyGeometry(Handle);
	}

} } // namespace Visera::Runtime