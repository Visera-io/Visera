module;
#include <Visera.h>
export module Visera.Runtime.Render.Scene;
import Visera.Runtime.Render.Scene.Light;
import Visera.Runtime.Render.Scene.Primitive;

import Visera.Runtime.Render.RTC;
import Visera.Runtime.Render.RHI;

import Visera.Core.Signal;
import Visera.Core.Media;
import Visera.Core.Type;

export namespace VE
{

	class FScene
	{
	public:
		using FPrimID = UInt32;
		struct FAttachment
		{
			FName                   Name;
			FPrimID                 ID;
			SharedPtr<IPrimitive>   Primitive;
		};

		static inline auto
		Create() { return CreateSharedPtr<FScene>(); }

		auto inline
		Attach(const FName& _Name, SharedPtr<const FModel> _Model) -> const FAttachment&;
		void inline
		Commit();

		void inline
		Accept(RTC::FRay* _Ray) const { _Ray->CastTo(AccelerationStructure); }

	private:
		SharedPtr<RTC::FAccelerationStructure> AccelerationStructure;
		
		HashMap<FName, FAttachment> AttachmentTable;
		
	public:
		FScene()  = default;
		~FScene() = default;
	};

	const FScene::FAttachment& FScene::
	Attach(const FName& _Name, SharedPtr<const FModel> _Model)
	{
		if (!AccelerationStructure)
		{ AccelerationStructure = RTC::FAccelerationStructure::Create(); }

		if (_Model->IsExpired())
		{ throw SRuntimeError(Text("Failed to add the Attachment({})! -- model was expired!", _Name.GetNameWithNumber())); }

		if (AttachmentTable.contains(_Name))
		{ throw SRuntimeError(Text("Failed to add the Attachment({})! -- Already exists!", _Name.GetNameWithNumber())); }

		auto& NewAttachment = AttachmentTable[_Name];
		NewAttachment.Name = _Name;
		NewAttachment.Primitive = FMeshPrimitive::Create(_Model);

		//Add a new node to the AS
		RTC::FAccelerationStructure::FNode::FCreateInfo ASNodeCreateInfo
		{
			.Topology        = RTC::ETopology::Triangle,
			.VerticesData	 = NewAttachment.Primitive->GetVerticesData(),
			.VerticesDataSize= NewAttachment.Primitive->GetCPUVertexBufferSize(),
			.IndicesData	 = NewAttachment.Primitive->GetIndicesData(),
			.IndicesDataSize = NewAttachment.Primitive->GetCPUIndexBufferSize()
		};
		NewAttachment.ID = AccelerationStructure->Attach(RTC::FSceneNode::Create(ASNodeCreateInfo));

		return NewAttachment;
	}

	void FScene::
	Commit()
	{
		AccelerationStructure->Update();
	}

} // namespace VE