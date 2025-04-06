module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Postprocessing;
import :GammaCorrection;

import Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.Shader;

export namespace VE
{

    class FURPPostprocessingPass : public RHI::FRenderPass
    {
    public:
        FURPPostprocessingPass();

        auto GetGammaCorrectionPipeline() const -> SharedPtr<const RHI::FRenderPipeline> { return GammaCorrectionPipeline; }

    private:
        SharedPtr<RHI::FDescriptorSetLayout>   DescriptorSetLayout;

        SharedPtr<RHI::FPipelineLayout>        PipelineLayout;
        SharedPtr<RHI::FRenderPipelineSetting> PipelineSetting;

        SharedPtr<RHI::FRenderPipeline>        GammaCorrectionPipeline;
    };

    FURPPostprocessingPass::
    FURPPostprocessingPass() : RHI::FRenderPass{ EType::Postprocessing }
    {
        DescriptorSetLayout = RHI::CreateDescriptorSetLayout()
            ->AddBinding(0, RHI::EDescriptorType::CombinedImageSampler, 1, RHI::EShaderStage::Fragment)
            ->Build();

        PipelineLayout  = RHI::CreatePipelineLayout()
            ->AddDescriptorSetLayout(DescriptorSetLayout)
            ->Build();

        PipelineSetting = RHI::CreateRenderPipelineSetting()
            ->SetCullMode(RHI::ECullMode::Disable)
            ->Confirm();

        GammaCorrectionPipeline = RHI::FRenderPipeline::Create(PipelineLayout, PipelineSetting,
            FShader::Create("URP-Postprocessing-GammaCorrection.slang", "VertexMain")->Compile()->GetCompiledShader(),
            FShader::Create("URP-Postprocessing-GammaCorrection.slang", "FragmentMain")->Compile()->GetCompiledShader());

        AddSubpass(FSubpass{
				.Pipeline = GammaCorrectionPipeline,

				.ColorImageReferences = {RHI::SV_Color},

				.SrcStage = RHI::EGraphicsPipelineStage::PipelineTop,
				.SrcStageAccess = RHI::EAccess::None,
				.DstStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
				.DstStageAccess = RHI::EAccess::W_ColorAttachment,});

    }

} // namespace VE