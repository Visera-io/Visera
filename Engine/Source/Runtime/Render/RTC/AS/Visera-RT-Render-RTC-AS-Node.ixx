module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.Render.RTC.AS:Node;
import Visera.Runtime.Render.RTC.Embree;

import Visera.Core.Signal;
import Visera.Core.Math.Basic;
import Visera.Core.OS.Memory;
import Visera.Core.Media.Model;

export namespace VE
{

    class FASNode
	{
	public:
		void Update()	    const { rtcCommitGeometry(Handle); }
		void Hide()		    const { rtcDisableGeometry(Handle); bVisible = False; }
		void Reveal()		const { rtcEnableGeometry(Handle);  bVisible = True; }
		Bool IsVisible()    const { return bVisible; }

		auto GetTopology()	const -> Embree::ETopology { return Topology; }
		auto GetHandle()	const -> const Embree::FGeometry { return Handle; }

		struct FCreateInfo
		{
			Embree::ETopology Topology	 = Embree::ETopology::None;
			const UInt64	  VertexCount= 0;
			const Float*	  Vertices	 = nullptr;
			const UInt64	  IndexCount = 0;
			const UInt32*	  Indices	 = nullptr;
		};
		
		FASNode() = delete;
		FASNode(const FCreateInfo& _CreateInfo);
		~FASNode();

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

	FASNode::
	FASNode(const FCreateInfo& _CreateInfo):
		Topology{_CreateInfo.Topology},
		VertexCount{_CreateInfo.VertexCount},
		IndexCount{_CreateInfo.IndexCount}
	{
		Handle = rtcNewGeometry(Embree::GetDevice()->GetHandle(), AutoCast(Topology));
		if (!Handle) { throw SRuntimeError("Failed to create the FASNode!"); }

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
			throw SRuntimeError("You MUST assign the topology when creating a FASNode!");
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

	FASNode::~FASNode()
	{
		if (Handle)
		{
			rtcReleaseGeometry(Handle);
		}
	}

}// namespace VE