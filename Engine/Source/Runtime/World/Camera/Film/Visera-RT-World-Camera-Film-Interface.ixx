module;
#include <Visera.h>
export module Visera.Runtime.World.Camera.Film:Interface;

export namespace VE
{

    class IFilm
    {
    public:
        struct FPixel
        {
            Float R{0}, G{0}, B{0}, A{1}; //[0,1]
        };

        struct FResolution
        {
            UInt32 Width;
            UInt32 Height;
        };

        auto GetSize() const -> UInt64 { return Pixels.size(); }

        IFilm() = delete;
        IFilm(FResolution _Resolution);
        ~IFilm() = default;

    protected:
        FResolution   Resolution;
        Array<FPixel> Pixels;
    };

    IFilm::
    IFilm(FResolution _Resolution)
        :
        Resolution(std::move(_Resolution)),
        Pixels(Resolution.Width * Resolution.Height)
    {

    }

} // namespace VE