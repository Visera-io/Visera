module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.Render.RTC.AS;
import :Node;

import Visera.Runtime.Render.RTC.Embree;

import Visera.Core.Signal;

export namespace VE
{

    class FAccelerationStructure
	{
	public:
        using FNode  = FASNode;
		using NodeID = UInt32;

        static inline auto
        Create() { return CreateSharedPtr<FAccelerationStructure>(); }

		void Update();
		auto Attach(SharedPtr<FNode> _Node, void* _UserData = nullptr) -> NodeID;

		auto GetHandle() const -> RTCScene		{ return Handle; }

	private:
		RTCScene				Handle = nullptr;
		
		HashMap<NodeID, SharedPtr<FNode>> NodeTable;

	public:
		FAccelerationStructure();
		~FAccelerationStructure();
	};

	FAccelerationStructure::NodeID FAccelerationStructure::
	Attach(SharedPtr<FNode> _Node, void* _UserData/* = nullptr*/)
	{
		NodeID ID = rtcAttachGeometry(Handle, _Node->GetHandle());

		rtcSetGeometryUserData(_Node->GetHandle(), _UserData);

		NodeTable[ID] = std::move(_Node);

		return ID;
	}

	void FAccelerationStructure::
	Update()
	{
		for (auto& [ID, Attachment]: NodeTable)
		{
			if (Attachment) Attachment->Update();
		}
		rtcCommitScene(Handle);
	}

	FAccelerationStructure::
	FAccelerationStructure()
	{
		Handle = rtcNewScene(Embree::GetDevice()->GetHandle());
		if (!Handle)
		{ throw SRuntimeError(Text("Failed to create the Embree Acceleration Structure!")); }
	}

	FAccelerationStructure::
	~FAccelerationStructure()
	{
		rtcReleaseScene(Handle);
	}

}// namespace VE