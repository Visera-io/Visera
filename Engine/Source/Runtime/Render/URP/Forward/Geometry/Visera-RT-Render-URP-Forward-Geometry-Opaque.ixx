module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Forward.Geometry:Opaque;

import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FURPOpaquePipeline : public RHI::FRenderPipeline
    {
    public:
        FURPOpaquePipeline() = delete;
        FURPOpaquePipeline(SharedPtr<const RHI::FPipelineLayout> _Layout,
                           SharedPtr<const RHI::FRenderPipelineSetting> _Settings,
                           SharedPtr<const RHI::FShader> _VertexShader,
                           SharedPtr<const RHI::FShader> _FragmentShader);
    };

    FURPOpaquePipeline::
    FURPOpaquePipeline(SharedPtr<const RHI::FPipelineLayout> _Layout,
                       SharedPtr<const RHI::FRenderPipelineSetting> _Settings,
                       SharedPtr<const RHI::FShader> _VertexShader,
                       SharedPtr<const RHI::FShader> _FragmentShader)
        : RHI::FRenderPipeline{_Layout, _Settings, _VertexShader, _FragmentShader}
    {

    }

} // namespace VE