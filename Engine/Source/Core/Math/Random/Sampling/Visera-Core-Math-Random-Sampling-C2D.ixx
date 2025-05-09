module;
#include <Visera.h>
export module Visera.Core.Math.Random.Sampling:C2D;

import Visera.Core.Math.Basic;

export namespace VE
{

    inline auto
    SampleUnitSquare(Float _Ux, Float _Uy) -> Vector2F
    {
        return { _Ux, _Uy };
    }

    inline auto
    SampleUnitDisk(Float _URadius, Float _UTheta) -> Vector2F
    {
        Float  Radius = Sqrt(_URadius);
        Radian Theta = 2 * PI * _UTheta;
        Float  X = Radius * Sin(Theta);
        Float  Y = Radius * Cos(Theta);
        return {X, Y};
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