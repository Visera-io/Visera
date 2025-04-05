module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Background:Skybox;

import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FURPSkyboxPipeline : public RHI::FRenderPipeline
    {
    public:
        FURPSkyboxPipeline() = delete;
        FURPSkyboxPipeline(SharedPtr<const RHI::FPipelineLayout> _Layout,
                           SharedPtr<const RHI::FRenderPipelineSetting> _Settings,
                           SharedPtr<const RHI::FSPIRVShader> _VertexShader,
                           SharedPtr<const RHI::FSPIRVShader> _FragmentShader);
    };

    FURPSkyboxPipeline::
    FURPSkyboxPipeline(SharedPtr<const RHI::FPipelineLayout> _Layout,
                       SharedPtr<const RHI::FRenderPipelineSetting> _Settings,
                       SharedPtr<const RHI::FSPIRVShader> _VertexShader,
                       SharedPtr<const RHI::FSPIRVShader> _FragmentShader)
        : RHI::FRenderPipeline{_Layout, _Settings, _VertexShader, _FragmentShader}
    {

    }

} // namespace VE