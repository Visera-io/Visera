module;
#include <Visera.h>
export module Visera.Core.Math.Random.Sampling:C2D;

import Visera.Core.Math.Basic;

export namespace VE
{

    inline auto
    SampleUnitSquare(Float& _Ux, Float& _Uy) -> Vector2F
    {
        return { _Ux, _Uy };
    }

    inline auto
    SampleUnitHemisphere(Float _UTheta, Float _UPhi) -> Vector3F
    {
        Radian Theta = ACos(_UTheta);
        Radian Phi   = 2.0 * PI * _UPhi;
        Float X = Sin(Phi) * Cos(Theta);
        Float Y = Sin(Phi) * Sin(Theta);
        Float Z = Cos(Theta);

        return {X, Y, Z};
    }

} // namespace VE