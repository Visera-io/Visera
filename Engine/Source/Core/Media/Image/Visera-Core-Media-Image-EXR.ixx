module;
#include <Visera.h>
#include <OpenEXR/ImfInputFile.h>
export module Visera.Core.Media.Image:EXR;
#define VE_MODULE_NAME "Image:EXR"
import :Interface;

import Visera.Core.Log;
import Visera.Core.Signal;

export namespace VE
{
    using namespace OPENEXR_IMF_NAMESPACE;  // Imf
    using namespace IMATH_NAMESPACE;        // Imath (for Box2i etc.)

    class FEXRImage : public IImage
    {
    public:
        FEXRImage() = delete;
        FEXRImage(const FPath& _Path);
    };

    FEXRImage::
    FEXRImage(const FPath& _Path) : IImage(EImageType::EXR, _Path)
    {
        String PlatformPath = _Path.ToPlatformString();
        try
        {
            InputFile ImageFile(PlatformPath.c_str());

            const Box2i DataWindow = ImageFile.header().dataWindow();
            Width    = DataWindow.max.x - DataWindow.min.x + 1;
            Height   = DataWindow.max.y - DataWindow.min.y + 1;
            Channels = 4;

        }
        catch (const std::exception& Error)
        {
            VE_LOG_ERROR("{} -- throw(SIOFailure).", Error.what());
            throw SIOFailure(Error.what());
        }

        Format = EImageFormat::RGBA_HalfFloat;

        VE_LOG_INFO("Created a new EXR image {}.", GetInfo());
    }

}