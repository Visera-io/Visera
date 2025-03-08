module;
#include <Visera.h>
export module Visera.Core.Math;
export import Visera.Core.Math.Basic;
export import Visera.Core.Math.Hash;

export namespace VE
{

    auto ComputeTriangleArea(const Vector3F& _Pa, const Vector3F& _Pb, const Vector3F& _Pc,) -> float
    {
        //[FIXME]:Not checked
        // A_triangle = 1/2 * Cross(Vab, Vac) = sqrt( ||Vab||^2 * ||Vac||^2 - (Dot(Vab, Vac)^2) )
        Vector3F Vab = _Pb - _Pa;
        Vector3F Vac = _Pc - _Pa;
        float DotVabVac = Vab.dot(Vac);
        float SqrArea = Vab.norm() * Vac.norm() - (DotVabVac * DotVabVac);
        return 0.5 * std::sqrt(SqrArea);
    }
	
} // namespace VE