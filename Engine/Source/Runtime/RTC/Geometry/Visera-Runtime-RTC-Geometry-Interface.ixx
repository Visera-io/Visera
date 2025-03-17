module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.RTC.Geometry:Interface;

import Visera.Runtime.RTC.Embree;
import Visera.Core.Signal;
import Visera.Core.System.Memory;

export namespace VE
{

    class IGeometry
	{
	public:
		void Update()	    const { rtcCommitGeometry(Handle); }
		void Hide()		    const { rtcDisableGeometry(Handle); bVisible = False; }
		void Reveal()		const { rtcEnableGeometry(Handle);  bVisible = True; }
		Bool IsVisible()    const { return bVisible; }

		auto GetTopology()	const -> Embree::ETopology { return Topology; }
		auto GetHandle()	const -> const Embree::FGeometry { return Handle; }

		void SetVertices(const Float* _Vertices, UInt32 _VertexCount = 0, UInt32 _Offset = 0) { Memory::Memcpy((Vertices + _Offset), _Vertices, (_VertexCount ? _VertexCount : VertexCount) * VertexByteSize); }
		void SetIndices(const UInt32* _Indices, UInt32 _IndexCount = 0, UInt32 _Offset = 0)   { Memory::Memcpy((Indices + _Offset), _Indices, (_IndexCount ? _IndexCount : IndexCount) * IndexByteSize); }

		struct FCreateInfo
		{
			Embree::ETopology	Topology	 = Embree::ETopology::None;
			UInt64		    VertexCount	 = 0;
			Float*		    Vertices		 = nullptr;
			UInt64		    IndexCount	 = 0;
			UInt32*		    Indices		 = nullptr;

		};
		IGeometry() = delete;
		IGeometry(const FCreateInfo& _CreateInfo);
		~IGeometry();

	protected:
		Embree::FGeometry  	Handle	 = nullptr;
		mutable Bool 		bVisible = True;
		Embree::ETopology	Topology = Embree::ETopology::None;

		Float*		  		Vertices       = nullptr;
		Embree::EFormat 	VertexFormat;
		UInt64		  		VertexCount    = 0;
		UInt32		  		VertexByteSize = 0;
		UInt32*		  		Indices        = nullptr;
		Embree::EFormat 	IndexFormat;
		UInt64		  		IndexCount	   = 0;
		UInt32		  		IndexByteSize  = 0;
	};

	IGeometry::
	IGeometry(const FCreateInfo& _CreateInfo):
		Topology{_CreateInfo.Topology},
		VertexCount{_CreateInfo.VertexCount},
		IndexCount{_CreateInfo.IndexCount}
	{
		Handle = rtcNewGeometry(Embree::GetDevice()->GetHandle(), AutoCast(Topology));
		if (!Handle) { throw SRuntimeError("Failed to create the IGeometry!"); }

		switch (Topology)
		{
		case Embree::ETopology::Triangle:
		{
			VertexFormat	= Embree::EFormat::Vector3F;
			VertexByteSize = sizeof(float) * 3;
			IndexFormat		= Embree::EFormat::TriangleIndices;
			IndexByteSize  = sizeof(UInt32) * 3;

			//Vertices = _CreateInfo.Vertices;
			//Indices = _CreateInfo.Indices;

			Vertices = (Float*)Memory::MallocNow(VertexCount * VertexByteSize, 0);
			if (!Vertices) { throw SRuntimeError("Failed to allocate Geometry Vertex Buffer!"); }
			if (_CreateInfo.Vertices) { Memory::Memcpy(Vertices, _CreateInfo.Vertices, VertexCount * VertexByteSize); }

			Indices = (UInt32*)Memory::MallocNow(IndexCount * IndexByteSize, 0);
			if (!Indices) { throw SRuntimeError("Failed to allocate Geometry Index Buffer!"); }
			if (_CreateInfo.Indices)  { Memory::Memcpy(Indices, _CreateInfo.Indices, IndexCount * IndexByteSize); }
			break;
		}
		case Embree::ETopology::Quad:
		{
			VE_ASSERT(VertexCount % 4 == 0);
			VE_WIP;
			break;
		}
		case Embree::ETopology::None:
		{
			throw SRuntimeError("You MUST assign the topology when creating a IGeometry!");
			break;
		}
		default:
			throw SRuntimeError("Unsupported Mesh Topology({})!", UInt32(Topology));
		}

        // Set Vertex Buffer
		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(Embree::EBufferType::Vertex),
			0,
			AutoCast(VertexFormat),
			Vertices,
			0,
			VertexByteSize,
			VertexCount);
        // Set Index Buffer
		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(Embree::EBufferType::Index),
			0,
			AutoCast(IndexFormat),
			Indices,
			0,
			IndexByteSize,
			IndexCount);
	}

	IGeometry::~IGeometry()
	{
		if (Handle)
		{
			rtcReleaseGeometry(Handle);
		}
	}

}// namespace VE