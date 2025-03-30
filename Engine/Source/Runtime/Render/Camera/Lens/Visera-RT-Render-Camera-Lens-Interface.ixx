module;
#include <Visera.h>
export module Visera.Runtime.Render.Camera.Lens:Interface;

import Visera.Core.Math.Basic;

export namespace VE
{

    class ILens
    {
    public:
        virtual auto
        Sample() -> Vector2F = 0;

        //auto GetFocalLength() const -> Float { return FocalLength; }
        //void SetFocalLength(Float _NewFocalLength) { FocalLength = _NewFocalLength; }

        ILens() = default;
        //ILens(Float _FocalLength) : FocalLength(_FocalLength) {}
        virtual ~ILens() = default;
    protected:
        //Float FocalLength = 0.0;
    };

} // namespace VE