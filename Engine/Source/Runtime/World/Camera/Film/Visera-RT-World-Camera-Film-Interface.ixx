module;
#include <Visera.h>
export module Visera.Runtime.World.Camera.Film:Interface;

import Visera.Core.Media.Image;

export namespace VE
{
    class FCamera;

    class IFilm
    {
        friend class FCamera;
    public:
        virtual auto
        Develop() -> const FImage& { return Negative; }

        auto GetSize()      const -> UInt64 { return Negative.GetSize(); }
        auto GetWidth()     const -> UInt32 { return Negative.GetWidth(); }
        auto GetHeight()    const -> UInt32 { return Negative.GetHeight(); }
        auto GetPixelCount()const -> UInt64 { return Negative.GetPixelCount(); }

        IFilm() = delete;
        IFilm(UInt32 _Width, UInt32 _Height);
        ~IFilm() = default;

    protected:
        FImage Negative;
    };

    IFilm::
    IFilm(UInt32 _Width, UInt32 _Height)
        :
        Negative{_Width, _Height}
    {

    }

} // namespace VE