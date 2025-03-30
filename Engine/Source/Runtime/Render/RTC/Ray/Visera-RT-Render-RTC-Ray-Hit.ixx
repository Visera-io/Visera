module;
#include <Visera.h>
#include <embree4/rtcore.h>
export module Visera.Runtime.Render.RTC.Ray:Hit;
import Visera.Runtime.Render.RTC.Embree;
import Visera.Core.Math.Basic;

export namespace VE
{

    class FHit
    {
    public:
        auto IsValid()                  const -> Bool       { return Details.geomID != Embree::InvalidGeometryID; }
        auto GetSurfaceNormal()         const -> Vector3F   { return Vector3F{Details.Ng_x, Details.Ng_y, Details.Ng_z }.normalized(); }
        auto GetHitBarycentricCoord()   const -> Vector2F   { return {Details.u, Details.v}; }
        auto GetHitGeometryID()         const -> UInt32     { return Details.geomID; }
        auto GetHitPrimitiveID()        const -> UInt32     { return Details.primID; }
        auto GetDetails()               const -> const Embree::FHit& { return Details; }

        FHit() : Details
        {
            .geomID =  RTC_INVALID_GEOMETRY_ID,
            .instID = {RTC_INVALID_GEOMETRY_ID}
        } {}

    private:
        Embree::FHit Details;
    };

}// namespace VE