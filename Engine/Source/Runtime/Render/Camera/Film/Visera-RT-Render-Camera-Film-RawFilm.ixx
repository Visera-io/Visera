module;
#include <Visera.h>
export module Visera.Runtime.Render.Camera.Film:RawFilm;
import :Interface;

export namespace VE
{

    class FRawFilm : public IFilm
    {
    public:
        FRawFilm() = delete;
        FRawFilm(FResolution _Resolution) : IFilm(_Resolution) {}
    };

} // namespace VE