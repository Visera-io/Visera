module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Postprocess;
import :GammaCorrection;

import Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.Shader;

export namespace VE
{

    class FURPPostprocessPass : public RHI::FRenderPass
    {
    public:
        FURPPostprocessPass();

        auto GetGammaCorrectionPipeline() const -> SharedPtr<const RHI::FRenderPipeline> { return GammaCorrectionPipeline; }

    private:
        SharedPtr<const RHI::FDescriptorSetLayout> SVColorDSLayout;

        SharedPtr<RHI::FPipelineLayout>        PipelineLayout;
        SharedPtr<RHI::FRenderPipelineSetting> PipelineSetting;

        SharedPtr<RHI::FRenderPipeline>        GammaCorrectionPipeline;
    };

    FURPPostprocessPass::
    FURPPostprocessPass() : RHI::FRenderPass{ EType::Postprocessing }
    {
        SVColorDSLayout = RHI::FFrameContext::SVColorTextureDSLayout;

        PipelineLayout  = RHI::CreatePipelineLayout()
            ->AddDescriptorSetLayout(SVColorDSLayout)
            ->Build();

        PipelineSetting = RHI::CreateRenderPipelineSetting()
            ->SetCullMode(RHI::ECullMode::Disable)
            ->Confirm();

        GammaCorrectionPipeline = RHI::FRenderPipeline::Create(PipelineLayout, PipelineSetting,
            FShader::Create("URP-Postprocess-GammaCorrection.slang", "VertexMain")->Compile()->GetCompiledShader(),
            FShader::Create("URP-Postprocess-GammaCorrection.slang", "FragmentMain")->Compile()->GetCompiledShader());

        AddSubpass(FSubpass{
				.Pipeline = GammaCorrectionPipeline,

				.ColorImageReferences = {RHI::SV_Color},

				.SrcStage = RHI::EGraphicsPipelineStage::PipelineTop,
				.SrcStageAccess = RHI::EAccess::None,
				.DstStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
				.DstStageAccess = RHI::EAccess::W_ColorAttachment,});

    }

} // namespace VE