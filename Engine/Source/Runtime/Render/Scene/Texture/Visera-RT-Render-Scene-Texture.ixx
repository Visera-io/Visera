module;
#include <Visera.h>
export module Visera.Runtime.Render.Scene.Texture;
#define VE_MODULE_NAME "Texture"

import Visera.Core.Log;
import Visera.Core.Media.Image;
import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FTexture2D
    {
    public:
        inline auto
        Create(SharedPtr<const FImage> _Image) { return CreateSharedPtr<FTexture2D>(_Image); }

        FTexture2D() = delete;
        FTexture2D(SharedPtr<const FImage> _Image);
        ~FTexture2D();

    private:
        WeakPtr<const FImage> ImageReference;
    };

    FTexture2D::
    FTexture2D(SharedPtr<const FImage> _Image)
        :ImageReference{_Image}
    {
        
    }

    FTexture2D::
    ~FTexture2D()
    {

    }

} // namespace VE