module;
#include <Visera.h>
export module Visera.Runtime.Render.Camera.Lens:DiskLens;
import :Interface;

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