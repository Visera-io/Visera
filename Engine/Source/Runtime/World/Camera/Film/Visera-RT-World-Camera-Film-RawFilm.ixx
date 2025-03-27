module;
#include <Visera.h>
export module Visera.Runtime.World.Camera.Film:RawFilm;
import :Interface;

export namespace VE
{

    class FRawFilm : public IFilm
    {
    public:
        FRawFilm() = delete;
        FRawFilm(UInt32 _Width, UInt32 _Height) : IFilm(_Width, _Height) {}
    };

} // namespace VE