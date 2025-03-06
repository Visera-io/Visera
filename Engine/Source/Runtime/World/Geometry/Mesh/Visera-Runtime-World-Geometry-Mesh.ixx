module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.World.Geometry.Mesh;

import Visera.Runtime.World.RTC;
import Visera.Core.Signal;
import Visera.Core.Math;
import Visera.Core.System.Memory;

export namespace VE
{
	
	class FMesh
	{
	public:
		using ETopology = RTC::ETopology;

		void Update()	    const { rtcCommitGeometry(Handle); }
		void Disable()		const { rtcDisableGeometry(Handle); bVisible = False; }
		void Enable()		const { rtcEnableGeometry(Handle);  bVisible = True; }
		Bool IsVisible()    const { return bVisible; }

		auto GetTopology()	const -> ETopology { return Topology; }
		auto GetHandle()	const -> const RTCGeometry { return Handle; }

		void SetVertices(const Float* _Vertices, UInt32 _VertexCount = 0, UInt32 _Offset = 0) { Memory::Memcpy((Vertices + _Offset), _Vertices, (_VertexCount ? _VertexCount : VertexCount) * VertexByteSize); }
		void SetIndices(const UInt32* _Indices, UInt32 _IndexCount = 0, UInt32 _Offset = 0) { Memory::Memcpy((Indices + _Offset), _Indices, (_IndexCount ? _IndexCount : IndexCount) * IndexByteSize); }

		struct FCreateInfo
		{
			ETopology	  Topology		 = ETopology::None;
			UInt64		  VertexCount	 = 0;
			Float*		  Vertices		 = nullptr;
			UInt64		  IndexCount	 = 0;
			UInt32*		  Indices		 = nullptr;

		};
		FMesh() = delete;
		FMesh(const FCreateInfo& _CreateInfo);
		~FMesh();

	protected:
		RTCGeometry  Handle			= nullptr;
		mutable Bool bVisible		= True;
		ETopology	 Topology		= ETopology::None;

		Float*		 Vertices		= nullptr;
		RTCFormat	 VertexFormat;
		UInt64		 VertexCount	= 0;
		UInt32		 VertexByteSize = 0;
		UInt32*		 Indices		= nullptr;
		RTCFormat    IndexFormat;
		UInt64		 IndexCount		= 0;
		UInt32		 IndexByteSize  = 0;
	};

	FMesh::
	FMesh(const FCreateInfo& _CreateInfo):
		Topology{_CreateInfo.Topology},
		VertexCount{_CreateInfo.VertexCount},
		IndexCount{_CreateInfo.IndexCount}
	{
		Handle = rtcNewGeometry(RTC::GetAPI()->GetDevice(), AutoCast(Topology));
		if (!Handle) { throw SRuntimeError("Failed to create the FMesh!"); }

		switch (Topology)
		{
		case ETopology::Triangle:
		{
			VE_ASSERT(VertexCount && VertexCount % 3 == 0);

			VertexFormat	= AutoCast(EEmbreeType::Vector3F);
			VertexByteSize = sizeof(Vector3F);
			IndexFormat		= AutoCast(EEmbreeType::TriangleIndices);
			IndexByteSize  = sizeof(UInt32) * 3;

			Vertices = (Float*)Memory::MallocNow(VertexCount * VertexByteSize, 0);
			if (!Vertices) { throw SRuntimeError("Failed to allocate Geometry Vertex FVulkanBuffer!"); }
			if (_CreateInfo.Vertices) { Memory::Memcpy(Vertices, _CreateInfo.Vertices, VertexCount * VertexByteSize); }

			Indices = (UInt32*)Memory::MallocNow(IndexCount * IndexByteSize, 0);
			if (!Indices) { throw SRuntimeError("Failed to allocate Geometry Index FVulkanBuffer!"); }
			if (_CreateInfo.Indices)  { Memory::Memcpy(Indices, _CreateInfo.Indices, IndexCount * IndexByteSize); }

			break;
		}
		case ETopology::Quad:
		{
			VE_ASSERT(VertexCount % 4 == 0);
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
					
		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(RTC::EBuffer::Vertex),
			0,
			VertexFormat,
			Vertices,
			0,
			VertexByteSize,
			VertexCount);

		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(RTC::EBuffer::Index),
			0,
			IndexFormat,
			Indices,
			0,
			IndexByteSize,
			IndexCount);

		Update();
	}

	FMesh::~FMesh()
	{
		if (Handle)
		{
			rtcReleaseGeometry(Handle);
		}
	}

}// namespace VE