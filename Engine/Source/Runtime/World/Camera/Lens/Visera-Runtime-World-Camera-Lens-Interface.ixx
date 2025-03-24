module;
#include <Visera.h>
export module Visera.Runtime.World.Camera.Lens:Interface;

export namespace VE
{

    class ILens
    {
    public:
        auto GetFocalLength() const -> Float { return FocalLength; }

        void SetFocalLength(Float _NewFocalLength) { FocalLength = _NewFocalLength; }

        ILens() = delete;
        ILens(Float _FocalLength) : FocalLength(_FocalLength) {}
        virtual ~ILens() = default;
    protected:
        Float FocalLength = 0.0;
    };

} // namespace VE