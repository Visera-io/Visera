module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.RTC.Ray:Hit;
import Visera.Runtime.RTC.Embree;
import Visera.Core.Math;

export namespace VE
{

    class FHit
    {
    public:
        auto IsValid()          const -> Bool       { return HitInfo.geomID != Embree::InvalidGeometryID; }
        auto GetSurfaceNormal() const -> Vector3F   { return Vector3F{HitInfo.Ng_x, HitInfo.Ng_y, HitInfo.Ng_z }.normalized(); }
        auto GetHitInfo()       const -> const Embree::FHit& { return HitInfo; }

        FHit() : HitInfo
        {
            .geomID =  RTC_INVALID_GEOMETRY_ID,
            .instID = {RTC_INVALID_GEOMETRY_ID}
        } {}

    private:
        Embree::FHit HitInfo;
    };

}// namespace VE