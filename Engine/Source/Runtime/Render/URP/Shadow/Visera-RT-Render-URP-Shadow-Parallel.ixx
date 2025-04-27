module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Shadow:Parallel;
#define VE_MODULE_NAME "Shadow:Parallel"
import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FURPParallelShaowPipeline : public RHI::FRenderPipeline
    {
    public:
        FURPParallelShaowPipeline() = delete;
        FURPParallelShaowPipeline(SharedPtr<const RHI::FPipelineLayout> _Layout,
                           SharedPtr<const RHI::FRenderPipelineSetting> _Settings,
                           SharedPtr<const RHI::FSPIRVShader>           _VertexShader,
                           SharedPtr<const RHI::FSPIRVShader>           _FragmentShader);
    };

    FURPParallelShaowPipeline::
    FURPParallelShaowPipeline(SharedPtr<const RHI::FPipelineLayout> _Layout,
                       SharedPtr<const RHI::FRenderPipelineSetting> _Settings,
                       SharedPtr<const RHI::FSPIRVShader>           _VertexShader,
                       SharedPtr<const RHI::FSPIRVShader>           _FragmentShader)
        : RHI::FRenderPipeline{_Layout, _Settings, _VertexShader, _FragmentShader}
    {

    }

} // namespace VE