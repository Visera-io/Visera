module;
#include <Visera.h>
export module Visera.Runtime.World.Camera.Lens:DiskLens;
import :Interface;

import Visera.Core.Math;

export namespace VE
{

    class FDiskLens : public ILens
    {
    public:
        FDiskLens() = delete;
        FDiskLens(Float _FocalLength) : ILens(_FocalLength) {}

    private:
    };

} // namespace VE