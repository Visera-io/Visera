module;
#include <Visera.h>
export module Visera.Runtime.Render.Camera.Film:Interface;

import Visera.Core.Media.Image;

export namespace VE
{
    class IFilm
    {
    public:
        virtual auto
        Develop() -> SharedPtr<FImage> { return Negative; } //[TODO]: New Interface return const FImage
        void
        Clear() { Negative->Clear(); }

        auto GetSize()      const -> UInt64 { return Negative->GetSize(); }
        auto GetWidth()     const -> UInt32 { return Negative->GetWidth(); }
        auto GetHeight()    const -> UInt32 { return Negative->GetHeight(); }
        auto GetPixelCount()const -> UInt64 { return Negative->GetPixelCount(); }

        IFilm() = delete;
        IFilm(UInt32 _Width, UInt32 _Height);
        ~IFilm() = default;

    protected:
        SharedPtr<FImage> Negative;
    };

    IFilm::
    IFilm(UInt32 _Width, UInt32 _Height)
        :
        Negative{CreateSharedPtr<FImage>(_Width, _Height)}
    {

    }

} // namespace VE