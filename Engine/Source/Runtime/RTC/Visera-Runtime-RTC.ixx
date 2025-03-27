module;
#include <Visera.h>
export module Visera.Runtime.RTC;

import Visera.Runtime.RTC.Embree;
import Visera.Runtime.RTC.Ray;
import Visera.Runtime.RTC.Scene;

import Visera.Core.Signal;
import Visera.Core.Media.Model;

export namespace VE
{

	class RTC
	{
		VE_MODULE_MANAGER_CLASS(RTC);
	public:
		using FRay				= FRay;
		using FScene			= FScene;
		using FSceneAttachment	= FScene::FAttachment;

		using EBufferType  = Embree::EBufferType;
		using ETopology    = Embree::ETopology;
		using EMeshFaceWinding = FMesh::EFaceWinding;

		static inline auto
		CreateScene(StringView _Name) -> SharedPtr<FScene> { return CreateSharedPtr<FScene>(_Name); }
		static inline auto
		CreateSceneAttachment(SharedPtr<const FModel> _Model) -> SharedPtr<FSceneAttachment> { return CreateSharedPtr<FMesh>(_Model); }

	//private:
		static inline auto
		Bootstrap() -> void { Embree::Bootstrap(); }
		static inline auto
		Terminate() -> void { Embree::Terminate(); }
	};

	String Text(const FRay& _Ray)
	{ return Text("Ray: {} -> {}", _Ray.GetOrigin(), _Ray.GetDirection()); }

}// namespace VE