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
			ETopology         Topology	     = ETopology::None;
			const Float*	  VerticesData	 = nullptr;
			const UInt64	  VerticesDataSize= 0;
			const UInt32*	  IndicesData	 = nullptr;
			const UInt64	  IndicesDataSize= 0;
		};
		static inline auto
		Create(const FCreateInfo& _CreateInfo) { return CreateSharedPtr<FASNode>(_CreateInfo); }

		void Update()	     const { rtcCommitGeometry(Handle); }
		void Hide()		     const { rtcDisableGeometry(Handle); bVisible = False; }
		void Reveal()		 const { rtcEnableGeometry(Handle);  bVisible = True; }
		Bool IsVisible()     const { return bVisible; }

		auto GetVertexData() const -> const Float*  { return SharedVertices; }
		auto GetIndexData()  const -> const UInt32* { return SharedIndices;  }
		auto GetTopology()	 const -> Embree::ETopology { return Topology; }
		auto GetHandle()	 const -> const Embree::FGeometry { return Handle; }

		FASNode() = delete;
		FASNode(const FCreateInfo& _CreateInfo);
		~FASNode();

	protected:
		Embree::FGeometry  	Handle	 = nullptr;
		mutable Bool 		bVisible = True;
		Embree::ETopology	Topology = Embree::ETopology::None;

		Embree::EFormat 	VertexFormat;
		const Float*		SharedVertices       = nullptr; //Lifecycle is managed by external system
		UInt64              VerticesDataSize      = 0;
		
		Embree::EFormat 	IndexFormat;
		const UInt32*		SharedIndices        = nullptr; //Lifecycle is managed by external system
		UInt64              IndicesDataSize      = 0;
	};

	FASNode::
	FASNode(const FCreateInfo& _CreateInfo)
		:
		Topology      {_CreateInfo.Topology   },
		SharedVertices{_CreateInfo.VerticesData},
		SharedIndices {_CreateInfo.IndicesData},
		VerticesDataSize{_CreateInfo.VerticesDataSize},
		IndicesDataSize{_CreateInfo.IndicesDataSize}
	{
		VE_ASSERT(Topology != ETopology::None);
		VE_ASSERT(SharedVertices && SharedIndices);

		Handle = rtcNewGeometry(Embree::GetDevice()->GetHandle(), AutoCast(Topology));
		if (!Handle) { throw SRuntimeError("Failed to create the FASNode!"); }

		UInt64 VertexByteStride = 0;
		UInt64 IndexByteStride  = 0;

		switch (Topology)
		{
		case Embree::ETopology::Triangle:
		{
			VertexFormat	= Embree::EFormat::Vector3F;
			IndexFormat		= Embree::EFormat::TriangleIndices;

			VertexByteStride = 3 * sizeof(Float);
			IndexByteStride  = 3 * sizeof(UInt32);
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
			throw SRuntimeError("Unsupported Mesh Topology({})!", UInt32(Topology));
		}

        // Set Vertex Buffer
		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(Embree::EBufferType::Vertex),
			0,
			AutoCast(VertexFormat),
			SharedVertices,
			0,
			VertexByteStride,
			VerticesDataSize / VertexByteStride);
        // Set Index Buffer
		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(Embree::EBufferType::Index),
			0,
			AutoCast(IndexFormat),
			SharedIndices,
			0,
			IndexByteStride,
			IndicesDataSize / IndexByteStride);
	}

	FASNode::~FASNode()
	{
		if (Handle)
		{ rtcReleaseGeometry(Handle); }
	}

}// namespace VE