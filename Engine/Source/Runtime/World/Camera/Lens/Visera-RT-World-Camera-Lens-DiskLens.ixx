module;
#include <Visera.h>
export module Visera.Runtime.World.Camera.Lens:DiskLens;
import :Interface;

import Visera.Core.Math.Random;

export namespace VE
{

    class FDiskLens : public ILens
    {
    public:
        virtual auto
        Sample() -> Vector2F override { return Radius * SampleUnitDisk(RNG.Random(), RNG.Random()); };

        FDiskLens(Float _Radius = 1.0) : Radius{_Radius} {}
        //FDiskLens(Float _FocalLength) : ILens(_FocalLength) {}
    private:
        Float Radius = 1.0;
        FMT19937 RNG; //[TODO]: PCG RNG
    };

} // namespace VE