module;
#include <Visera.h>
export module Visera.Runtime.Render.Camera.Lens:Pinhole;
import :Interface;

export namespace VE
{

    class FPinhole : public ILens
    {
    public:
        virtual auto
        Sample() -> Vector2F override { return {0, 0}; };

        FPinhole() = default;
        //FPinhole(Float _FocalLength) : ILens(_FocalLength) {}
    };

} // namespace VE