module;
#include <Visera.h>
export module Visera.Runtime.Render.Scene;
import Visera.Runtime.Render.Scene.Light;
import Visera.Runtime.Render.Scene.Primitive;

import Visera.Runtime.Render.RTC;

import Visera.Core.Media.Model;

export namespace VE
{

	class FScene
	{
	public:
		static inline auto
		Create() { return CreateSharedPtr<FScene>(); }

		using FPrimID = UInt32;

		auto AttachPrimitive(SharedPtr<const FModel> _Model) -> FPrimID;
		void Commit();

		void Accept(RTC::FRay* _Ray) const { _Ray->CastTo(AccelerationStructure); }

	private:
		//CPU Raytracing
		SharedPtr<RTC::FAccelerationStructure> AccelerationStructure;
		//GPU Rasterization
		//WIP
			
	public:
		FScene()  = default;
		~FScene() = default;
	};

	FScene::FPrimID FScene::
	AttachPrimitive(SharedPtr<const FModel> _Model)
	{
		if (!AccelerationStructure)
		{ AccelerationStructure = RTC::FAccelerationStructure::Create(); }

		RTC::FAccelerationStructure::FNode::FCreateInfo CreateInfo
		{
			.Topology    = RTC::ETopology::Triangle,
			.VertexCount = _Model->GetVertexCount(),
			.Vertices	 = _Model->GetVertexData(),
			.IndexCount	 = _Model->GetIndexCount(),
			.Indices	 = _Model->GetIndexData(),
		};
		auto NewNode = CreateSharedPtr<RTC::FAccelerationStructure::FNode>(CreateInfo);

		return AccelerationStructure->Attach(NewNode);
	}

	void FScene::
	Commit()
	{
		AccelerationStructure->Update();
	}

} // namespace VE