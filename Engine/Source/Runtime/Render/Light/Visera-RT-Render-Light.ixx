module;
#include <Visera.h>
export module Visera.Runtime.Render.Light;
#define VE_MODULE_NAME "Light"

import Visera.Core.Math.Basic;
import Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.Texture;
import Visera.Runtime.Render.Camera;

export namespace VE
{

    class FLight
    {
    public:
        FLight();

    private:
        FCamera                      Camera;
        Array<SharedPtr<FShadowMap>> ShadowMaps;
    };

    FLight::
    FLight()
    {
        ShadowMaps = FShadowMap::BatchCreate(
            RHI::GetSwapchainFrameCount(),
            RHI::FFrameContext::RenderArea.extent.width,
            RHI::FFrameContext::RenderArea.extent.height);
    }

} // namespace VE