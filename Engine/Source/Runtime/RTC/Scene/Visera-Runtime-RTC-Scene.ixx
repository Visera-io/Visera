module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.RTC.Scene;

import Visera.Runtime.RTC.Embree;
import Visera.Runtime.RTC.Geometry;

import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;

export namespace VE
{

	class FScene
	{
	public:
		using AttachmentID = UInt32;
		void Update();
		auto Attach(SharedPtr<IGeometry> _Geometry) -> AttachmentID;

		auto GetHandle() const -> RTCScene		{ return Handle; }

	private:
		String					Name;
		RTCScene				Handle = nullptr;
		
		HashMap<AttachmentID, SharedPtr<IGeometry>> AttachmentTable;

	public:
		FScene(StringView _Name);
		FScene() = delete;
		~FScene();
	};

	FScene::AttachmentID FScene::
	Attach(SharedPtr<IGeometry> _Geometry)
	{
		//Log::Debug("Attaching a new Geometry({}) to Scene({})", _Geometry->GetHandle(), )
		AttachmentID ID = rtcAttachGeometry(Handle, _Geometry->GetHandle());
		AttachmentTable[ID] = std::move(_Geometry);
		return ID;
	}

	void FScene::
	Update()
	{
		for (auto& [ID, Geometry]: AttachmentTable)
		{
			if (Geometry) Geometry->Update();
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