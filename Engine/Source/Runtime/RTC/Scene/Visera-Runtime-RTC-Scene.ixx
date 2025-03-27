module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.RTC.Scene;
export import Visera.Runtime.RTC.Scene.Geometry;

import Visera.Runtime.RTC.Embree;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.Media.Model;

export namespace VE
{

	class FScene
	{
	public:
		using FAttachment = IGeometry;
		using AttachmentID = UInt32;

		void Update();
		auto Attach(SharedPtr<FAttachment> _Attachment) -> AttachmentID;

		auto GetHandle() const -> RTCScene		{ return Handle; }

	private:
		String					Name;
		RTCScene				Handle = nullptr;
		
		HashMap<AttachmentID, SharedPtr<FAttachment>> AttachmentTable;

	public:
		FScene(StringView _Name);
		FScene() = delete;
		~FScene();
	};

	FScene::AttachmentID FScene::
	Attach(SharedPtr<FAttachment> _Attachment)
	{
		//Log::Debug("Attaching a new Geometry({}) to Scene({})", _Geometry->GetHandle(), )
		AttachmentID ID = rtcAttachGeometry(Handle, _Attachment->GetHandle());
		AttachmentTable[ID] = std::move(_Attachment);
		return ID;
	}

	void FScene::
	Update()
	{
		for (auto& [ID, Attachment]: AttachmentTable)
		{
			if (Attachment) Attachment->Update();
		}
		rtcCommitScene(Handle);
	}

	FScene::
	FScene(StringView _Name)
		: Name{_Name}
	{
		Handle = rtcNewScene(Embree::GetDevice()->GetHandle());
		if (!Handle)
		{ throw SRuntimeError(Text("Failed to create Embree Scene({})!", Name)); }
	}

	FScene::
	~FScene()
	{
		Log::Debug("Destroying Scene({})...",  Name);
		rtcReleaseScene(Handle);
	}

} // namespace VE