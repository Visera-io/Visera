module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.Render.RTC.AS:Node;
import Visera.Runtime.Render.RTC.Embree;

import Visera.Core.Signal;
import Visera.Core.Math.Basic;
import Visera.Core.Media.Model;

export namespace VE
{

    class FASNode
	{
	public:
		using ETopology = Embree::ETopology;

		struct FCreateInfo
		{
			ETopology         Topology	   = ETopology::None;
			const void*	      Vertices     = nullptr;
			const UInt64      VertexCount  = 0;
			const UInt64	  VertexStride = 0;
			const UInt64      VertexOffset = 0; // FASNode only needs Vertex::Position
			const void*	      Indices      = nullptr;
			const UInt64      IndexCount   = 0;
			const UInt64	  IndexStride  = 0;
			const UInt64      IndexOffset  = 0;
		};
		static inline auto
		Create(const FCreateInfo& _CreateInfo) { return CreateSharedPtr<FASNode>(_CreateInfo); }

		void Update()	     const { rtcCommitGeometry(Handle); }
		void Hide()		     const { rtcDisableGeometry(Handle); bVisible = False; }
		void Reveal()		 const { rtcEnableGeometry(Handle);  bVisible = True; }
		Bool IsVisible()     const { return bVisible; }

		auto GetTopology()	 const -> Embree::ETopology { return Details.Topology; }
		auto GetHandle()	 const -> const Embree::FGeometry { return Handle; }

		FASNode() = delete;
		FASNode(const FCreateInfo& _CreateInfo);
		~FASNode();

	protected:
		Embree::FGeometry  	Handle	 = nullptr;

		Embree::EFormat 	VertexFormat;
		Embree::EFormat 	IndexFormat;
		const FCreateInfo   Details;

		mutable Bool 		bVisible = True;
	};

	FASNode::
	FASNode(const FCreateInfo& _CreateInfo)
		: Details{ _CreateInfo }
	{
		VE_ASSERT(Details.Topology != ETopology::None);
		VE_ASSERT(Details.Vertices && Details.VertexCount);

		Handle = rtcNewGeometry(Embree::GetDevice()->GetHandle(), AutoCast(Details.Topology));
		if (!Handle) { throw SRuntimeError("Failed to create the FASNode!"); }

		UInt32 IndexStrideScale = 1;

		switch (Details.Topology)
		{
		case Embree::ETopology::Triangle:
		{
			VertexFormat	= Embree::EFormat::Vector3F;
			IndexFormat		= Embree::EFormat::TriangleIndices;
			VE_ASSERT(Details.IndexCount % 3 == 0);
			IndexStrideScale = 3;
			break;
		}
		case Embree::ETopology::Quad:
		{
			VE_WIP;
			break;
		}
		case Embree::ETopology::None:
		{
			throw SRuntimeError("You MUST assign the topology when creating a FASNode!");
			break;
		}
		default:
			throw SRuntimeError("Unsupported Mesh Topology({})!", UInt32(Details.Topology));
		}

        // Set Vertex Buffer
		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(Embree::EBufferType::Vertex),
			0,
			AutoCast(VertexFormat),
			Details.Vertices,
			Details.VertexOffset,
			Details.VertexStride,
			Details.VertexCount);
        // Set Index Buffer
		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(Embree::EBufferType::Index),
			0,
			AutoCast(IndexFormat),
			Details.Indices,
			Details.IndexOffset,
			Details.IndexStride * IndexStrideScale,
			Details.IndexCount  / IndexStrideScale);
	}

	FASNode::~FASNode()
	{
		if (Handle)
		{ rtcReleaseGeometry(Handle); }
	}

}// namespace VE