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
		void Update();
		void Attach(SharedPtr<IGeometry> _Geometry);

		auto GetHandle() const -> RTCScene		{ return Handle; }

	private:
		String					Name;
		RTCScene				Handle = nullptr;

		Array<SharedPtr<IGeometry>> Geometries;

	public:
		FScene(StringView _Name);
		FScene() = delete;
		~FScene();

	};

	void FScene::
	Attach(SharedPtr<IGeometry> _Geometry)
	{
		//Log::Debug("Attaching a new Geometry({}) to Scene({})", _Geometry->GetHandle(), )
		rtcAttachGeometry(Handle, _Geometry->GetHandle());
		Geometries.emplace_back(std::move(_Geometry));
	}

	void FScene::
	Update()
	{
		for (auto& Geometry : Geometries)
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