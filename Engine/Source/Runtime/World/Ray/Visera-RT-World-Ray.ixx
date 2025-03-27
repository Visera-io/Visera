module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.World.Ray;
import :Hit;
import Visera.Runtime.Render.RTC.Embree;
import Visera.Runtime.World.Stage.Scene;
import Visera.Core.Math.Basic;

export namespace VE
{
	class FRay
	{
		//[TODO]: BatchCast (rtcIntersectN)
	public:
		void CastTo(const Embree::FScene _Scene)	{ VE_ASSERT(_Scene); rtcIntersect1(_Scene, reinterpret_cast<Embree::FRayHit*>(&Context)); }
		void CastTo(const FScene& _Scene)			{ CastTo(_Scene.GetHandle()); }
		void CastTo(SharedPtr<const FScene> _Scene) { CastTo(_Scene->GetHandle()); }

		Bool HasHit()		const { return Context.Hit.IsValid(); }
		Bool HasHitSurface()const { return GetDirection().dot(GetHitInfo().GetSurfaceNormal()) < 0.0; }
		auto GetHitInfo()	const -> const FHit&	{ return Context.Hit; }
		auto GetHitPoint()	const -> Vector3F		{ return GetOrigin() + Context.Ray.tfar * GetDirection(); }

		auto Reflect()		const -> FRay;

		auto GetOrigin()	const -> Vector3F	{ return { Context.Ray.org_x, Context.Ray.org_y, Context.Ray.org_z} ; }
		auto GetDirection() const -> Vector3F	{ return { Context.Ray.dir_x, Context.Ray.dir_y, Context.Ray.dir_z} ; }

		FRay() = default;
		FRay(const Vector3F& _Origin, const Vector3F& _Direction, Float _Near = Epsilon<Float>(), Float _Far = UpperBound<Float>());

	private:
		struct FRayContext
		{
			Embree::FRay Ray;
			FHit Hit;
		}Context;
		static_assert(sizeof(FRayContext) == sizeof(Embree::FRayHit));
	};

	FRay::
	FRay(const Vector3F& _Origin,
		const Vector3F& _Direction,
		Float _Near/* = Epsilon<Float>()*/,
		Float _Far/* = UpperBound<Float>()*/)
		:
		Context
		{
			.Ray {
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
			}
		}
	{
		VE_ASSERT(_Near < _Far);
	}

	FRay FRay::
	Reflect() const
	{
		VE_ASSERT(HasHit());

		Vector3F Origin		= GetOrigin();
		Vector3F IncDir		= GetDirection();
		Vector3F Normal		= Context.Hit.GetSurfaceNormal();

		Vector3F HitPoint	= Origin + Context.Ray.tfar * IncDir;
		Float	 Cosine		= IncDir.dot(Normal);
		VE_ASSERT(Cosine >= 0);
		Vector3F RefDir		= IncDir - 2 * (Cosine * Normal);

		return { HitPoint, RefDir };
	}

}// namespace VE