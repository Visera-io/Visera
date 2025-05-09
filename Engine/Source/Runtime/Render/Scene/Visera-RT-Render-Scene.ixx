module;
#include <Visera.h>
export module Visera.Runtime.Render.Scene;
#define VE_MODULE_NAME "Scene"
import Visera.Runtime.Render.Scene.Primitive;

import Visera.Runtime.Render.RTC;
import Visera.Runtime.Render.RHI;

import Visera.Core.Log;
import Visera.Core.Signal;
import Visera.Core.Media;
import Visera.Core.Type;

export namespace VE
{

	class FScene
	{
	public:
		using FPrimID = UInt32;

		enum EAttachmentLoadOp
		{
			None,
			FlipFaceWinding
		};

		class FAttachment
		{
			friend class FScene;
		public:
			auto GetName()      const -> const FName& { return Name; }
			auto GetID()        const -> FPrimID      { return ID; }
			auto GetPrimitive() const -> SharedPtr<const IPrimitive> { return Primitive; }

		private:
			FName                   Name;
			FPrimID                 ID;
			SharedPtr<IPrimitive>   Primitive;
		};

		static inline auto
		Create() { return CreateSharedPtr<FScene>(); }

		auto inline
		Attach(const FName& _Name, SharedPtr<const FModel> _Model, EAttachmentLoadOp _LoadOps = None) -> const FAttachment&;
		void inline
		Commit();

		Bool inline
		IsCommitted() const  { return bCommitted; }

		void inline
		Accept(RTC::FRay* _Ray) const { _Ray->CastTo(AccelerationStructure); }

	private:
		SharedPtr<RTC::FAccelerationStructure> AccelerationStructure;
		
		HashMap<FName, FAttachment> AttachmentTable;

		Bool bCommitted = False;
		
	public:
		FScene()  = default;
		~FScene() { if(!IsCommitted()) { VE_LOG_WARN("You may forget to commit the scene after attaching!"); } }
	};

	const FScene::FAttachment& FScene::
	Attach(const FName&            _Name,
		   SharedPtr<const FModel> _Model,
		   EAttachmentLoadOp       _LoadOps /*= None*/)
	{
		Log::Trace("Attaching a new premitive {} to the scene.", _Name.GetNameWithNumber());

		if (!AccelerationStructure)
		{ AccelerationStructure = RTC::FAccelerationStructure::Create(); }

		if (_Model->IsExpired())
		{ throw SRuntimeError(Text("Failed to add the Attachment({})! -- model was expired!", _Name.GetNameWithNumber())); }

		if (AttachmentTable.contains(_Name))
		{ throw SRuntimeError(Text("Failed to add the Attachment({})! -- Already exists!", _Name.GetNameWithNumber())); }

		auto MeshPrimitive = FMeshPrimitive::Create(_Model);
		if (_LoadOps & FlipFaceWinding)
		{
			MeshPrimitive->FlipFaceWinding();
		}
		MeshPrimitive->UploadToGPU();
		auto& NewAttachment = AttachmentTable[_Name];
		NewAttachment.Name = _Name;
		NewAttachment.Primitive = MeshPrimitive;

		auto Mesh = _Model->GetMesh(0);
		
		//Add a new node to the AS
		RTC::FAccelerationStructure::FNode::FCreateInfo ASNodeCreateInfo
		{
			.Topology        = RTC::ETopology::Triangle,
			.Vertices        = MeshPrimitive->GetVertexData(),
			.VertexCount     = MeshPrimitive->GetVertexCount(),
		    .VertexStride    = MeshPrimitive->GetVertexByteSize(),
			.VertexOffset    = 0,
			.Indices         = MeshPrimitive->GetIndexData(),
			.IndexCount      = MeshPrimitive->GetIndexCount(),
			.IndexStride     = MeshPrimitive->GetIndexByteSize(),
			.IndexOffset     = 0,
		};
		NewAttachment.ID = AccelerationStructure->Attach(RTC::FSceneNode::Create(ASNodeCreateInfo));

		bCommitted = False;
		return NewAttachment;
	}

	void FScene::
	Commit()
	{
		AccelerationStructure->Update();
		bCommitted = True;
	}

} // namespace VE