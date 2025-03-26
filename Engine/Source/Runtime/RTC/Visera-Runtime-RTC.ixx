module;
#include <Visera.h>
export module Visera.Runtime.RTC;

import Visera.Runtime.RTC.Embree;
import Visera.Runtime.RTC.Ray;
import Visera.Runtime.RTC.Geometry;
import Visera.Runtime.RTC.Scene;

import Visera.Core.Signal;
import Visera.Core.Media.Model;

export namespace VE
{

	class RTC
	{
		VE_MODULE_MANAGER_CLASS(RTC);
	public:
		using FRay		= FRay;
		using FScene	= FScene;
		using FMesh		= FMesh;

		using EBufferType  = Embree::EBufferType;
		using ETopology    = Embree::ETopology;
		using EMeshFaceWinding = FMesh::EFaceWinding;

		static inline auto
		CreateMesh(SharedPtr<const FModel> _Model) -> SharedPtr<FMesh> { return CreateSharedPtr<FMesh>(_Model); }
		static inline auto
		CreateScene(StringView _Name) -> SharedPtr<FScene> { return CreateSharedPtr<FScene>(_Name); }

	//private:
		static inline auto
		Bootstrap() -> void { Embree::Bootstrap(); }
		static inline auto
		Terminate() -> void { Embree::Terminate(); }
	};

	String Text(const FRay& _Ray)
	{ return Text("Ray: {} -> {}", _Ray.GetOrigin(), _Ray.GetDirection()); }

}// namespace VE