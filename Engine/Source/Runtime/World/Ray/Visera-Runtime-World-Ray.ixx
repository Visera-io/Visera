module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.World.Ray;
import Visera.Runtime.World.RTC;
import Visera.Runtime.World.Stage;
import Visera.Core.Math;

export namespace VE { namespace Runtime
{

	class FRay
	{
		//[TODO]: BatchCast (rtcIntersectN)
	public:
		Bool CastTo(SharedPtr<FScene> _Scene);
		Bool HasHit() const { return Handle.hit.geomID != RTC_INVALID_GEOMETRY_ID; }
		auto GetHit() const -> const RTCHit& { return Handle.hit; }

		FRay() = default;
		FRay(const Vector3F& _Origin, const Vector3F& _Direction, Float _Near = Epsilon<Float>(), Float _Far = UpperBound<Float>());

	private:
		RTCRayHit Handle{ 0 };
	};

	FRay::
	FRay(const Vector3F& _Origin,
		const Vector3F& _Direction,
		Float _Near/* = Epsilon<Float>()*/,
		Float _Far/* = UpperBound<Float>()*/)
		:
		Handle
		{
			.ray {
				.org_x = _Origin.x(),
				.org_y = _Origin.y(),
				.org_z = _Origin.z(),
				.tnear = _Near,
				.dir_x = _Direction.x(),
				.dir_y = _Direction.y(),
				.dir_z = _Direction.z(),
				.tfar  = _Far,
				.mask  = ~0U,
				.flags = 0x0,
			},
			.hit{
				.geomID =  RTC_INVALID_GEOMETRY_ID,
				.instID = {RTC_INVALID_GEOMETRY_ID}
			}
		}
	{
		VE_ASSERT(_Near < _Far);
	}

	Bool FRay::
	CastTo(SharedPtr<FScene> _Scene)
	{
		rtcIntersect1(_Scene->GetHandle(), &Handle);
		return HasHit();
	}

} } // namespace Visera::Runtime