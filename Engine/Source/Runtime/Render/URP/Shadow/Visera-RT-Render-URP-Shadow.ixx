module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Shadow;
#define VE_MODULE_NAME "Shadow"
import :Parallel;

import Visera.Core.Math.Basic;
import Visera.Core.OS.FileSystem;
import Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.Shader;
import Visera.Runtime.Render.Scene.Primitive;

export namespace VE
{
    
    class FURPShadowPass : public RHI::FRenderPass
    {
    public:
        struct FShadowPassShaderUBO
        {
            Matrix4x4F MVPMatrix;
        };
        auto GetUBOLayout() const -> SharedPtr<const RHI::FDescriptorSetLayout>  { return TransformsDSLayout; }

        FURPShadowPass();
        auto GetParallelShadowPipeline() const -> SharedPtr<const RHI::FRenderPipeline> { return ParallelShadowPipeline; }

    private:
        //Resources
        SharedPtr<const RHI::FDescriptorSetLayout> TransformsDSLayout;
        //Layouts
        SharedPtr<RHI::FPipelineLayout>        PipelineLayout;
        SharedPtr<RHI::FRenderPipelineSetting> PipelineSetting;
        //Pipelines
        SharedPtr<RHI::FRenderPipeline>        ParallelShadowPipeline;
    };

    FURPShadowPass::
    FURPShadowPass() : RHI::FRenderPass{ EType::Shadow }
    {
        TransformsDSLayout = RHI::FFrameContext::MatrixUBODSLayout;

        PipelineLayout = RHI::CreatePipelineLayout()
            ->AddDescriptorSetLayout(TransformsDSLayout)
            ->Build();
        
        PipelineSetting = RHI::CreateRenderPipelineSetting()
            ->EnableDepthTest()
            ->SetCullMode(RHI::ECullMode::Disable)
            ->Confirm();

        PipelineSetting->SetVertexInputState(RHI::FRenderPipelineSetting::FVertexInputDescription
            {
                .Binding = 0,
                .Size    = sizeof(FMeshPrimitive::FVertex),
                .Attributes =
                {
                    {
                        .Location = 0,
                        .Format   = RHI::EFormat::Vertex3F,
                        .Offset   = offsetof(FMeshPrimitive::FVertex, Position),
                    }
                }
            });

        ParallelShadowPipeline = RHI::FRenderPipeline::Create(PipelineLayout, PipelineSetting,
            FShader::Create("URP-Shadow-Parallel.slang", "VertexMain")->Compile()->GetCompiledShader(),
            FShader::Create("URP-Shadow-Parallel.slang", "FragmentMain")->Compile()->GetCompiledShader());

        AddSubpass(FSubpass{
				.Pipeline = ParallelShadowPipeline,

				.ColorImageReferences = {},

				.SrcStage = RHI::EGraphicsPipelineStage::PipelineTop,
				.SrcStageAccess = RHI::EAccess::None,
				.DstStage = RHI::EGraphicsPipelineStage::LateFragmentTests,
				.DstStageAccess = RHI::EAccess::W_DepthStencilAttachment,});
    }

} // namespace VE