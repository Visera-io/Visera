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
		struct FCreateInfo
		{
			Embree::ETopology Topology	 = Embree::ETopology::None;
			const Float*	  VerticeData	 = nullptr;
			const UInt64	  VerticeDataSize= 0;
			const UInt32*	  IndicesData	 = nullptr;
			const UInt64	  IndicesDataSize= 0;
		};
		static inline auto
		Create(const FCreateInfo& _CreateInfo) { return CreateSharedPtr<FASNode>(_CreateInfo); }

		void Update()	     const { rtcCommitGeometry(Handle); }
		void Hide()		     const { rtcDisableGeometry(Handle); bVisible = False; }
		void Reveal()		 const { rtcEnableGeometry(Handle);  bVisible = True; }
		Bool IsVisible()     const { return bVisible; }

		auto GetVertexData() const -> const Float*  { return Vertices; }
		auto GetIndexData()  const -> const UInt32* { return Indices;  }
		auto GetTopology()	 const -> Embree::ETopology { return Topology; }
		auto GetHandle()	 const -> const Embree::FGeometry { return Handle; }

		FASNode() = delete;
		FASNode(const FCreateInfo& _CreateInfo);
		~FASNode();

	protected:
		Embree::FGeometry  	Handle	 = nullptr;
		mutable Bool 		bVisible = True;
		Embree::ETopology	Topology = Embree::ETopology::None;

		Float*		  		Vertices       = nullptr;
		Embree::EFormat 	VertexFormat;
		UInt32*		  		Indices        = nullptr;
		Embree::EFormat 	IndexFormat;
	};

	FASNode::
	FASNode(const FCreateInfo& _CreateInfo):
		Topology{_CreateInfo.Topology}
	{
		Handle = rtcNewGeometry(Embree::GetDevice()->GetHandle(), AutoCast(Topology));
		if (!Handle) { throw SRuntimeError("Failed to create the FASNode!"); }

		switch (Topology)
		{
		case Embree::ETopology::Triangle:
		{
			VertexFormat	= Embree::EFormat::Vector3F;
			IndexFormat		= Embree::EFormat::TriangleIndices;

			//Vertices = _CreateInfo.Vertices;
			//Indices = _CreateInfo.Indices;

			Vertices = (Float*)Memory::MallocNow(_CreateInfo.VerticeDataSize, 0);
			if (!Vertices) { throw SRuntimeError("Failed to allocate Geometry Vertex Buffer!"); }
			if (_CreateInfo.VerticeData) { Memory::Memcpy(Vertices, _CreateInfo.VerticeData, _CreateInfo.VerticeDataSize); }

			Indices = (UInt32*)Memory::MallocNow(_CreateInfo.IndicesDataSize, 0);
			if (!Indices) { throw SRuntimeError("Failed to allocate Geometry Index Buffer!"); }
			if (_CreateInfo.IndicesData)  { Memory::Memcpy(Indices, _CreateInfo.IndicesData, _CreateInfo.IndicesDataSize); }
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

		//[FIXME]: HardCoded for now!!!!!!!

        // Set Vertex Buffer
		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(Embree::EBufferType::Vertex),
			0,
			AutoCast(VertexFormat),
			Vertices,
			0,
			3 * sizeof(float),
			_CreateInfo.VerticeDataSize / (3 * sizeof(float)));
        // Set Index Buffer
		rtcSetSharedGeometryBuffer(
			Handle,
			AutoCast(Embree::EBufferType::Index),
			0,
			AutoCast(IndexFormat),
			Indices,
			0,
			3 * sizeof(UInt32),
			_CreateInfo.IndicesDataSize / (3 *sizeof(UInt32)));
	}

	FASNode::~FASNode()
	{
		if (Handle)
		{ rtcReleaseGeometry(Handle); }
	}

}// namespace VE