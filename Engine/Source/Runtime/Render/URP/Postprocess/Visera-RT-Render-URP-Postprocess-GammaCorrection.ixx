module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Postprocess:GammaCorrection;

import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FURPGammaCorrectionPipeline : public RHI::FRenderPipeline
    {
    public:
        FURPGammaCorrectionPipeline() = delete;
        FURPGammaCorrectionPipeline(SharedPtr<const RHI::FPipelineLayout> _Layout,
                           SharedPtr<const RHI::FRenderPipelineSetting> _Settings,
                           SharedPtr<const RHI::FSPIRVShader> _VertexShader,
                           SharedPtr<const RHI::FSPIRVShader> _FragmentShader);
    };

    FURPGammaCorrectionPipeline::
    FURPGammaCorrectionPipeline(SharedPtr<const RHI::FPipelineLayout> _Layout,
                       SharedPtr<const RHI::FRenderPipelineSetting> _Settings,
                       SharedPtr<const RHI::FSPIRVShader> _VertexShader,
                       SharedPtr<const RHI::FSPIRVShader> _FragmentShader)
        : RHI::FRenderPipeline{_Layout, _Settings, _VertexShader, _FragmentShader}
    {

    }

} // namespace VE