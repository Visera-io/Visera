module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Background;
import :Skybox;

import Visera.Core.OS.FileSystem;

import Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.Shader;

export namespace VE
{

    class FURPBackgroundPass : public RHI::FRenderPass
    {
    public:
        FURPBackgroundPass();

        auto GetSkyboxPipeline() const -> SharedPtr<const RHI::FRenderPipeline> { return SkyboxPipeline; }

    private:
        SharedPtr<RHI::FPipelineLayout>        PipelineLayout;
        SharedPtr<RHI::FRenderPipelineSetting> PipelineSetting;

        SharedPtr<RHI::FRenderPipeline>        SkyboxPipeline;
    };

    FURPBackgroundPass::
    FURPBackgroundPass() : RHI::FRenderPass{ EType::Background }
    {
        PipelineLayout  = RHI::CreatePipelineLayout()
            ->Build();
        PipelineSetting = RHI::CreateRenderPipelineSetting()
            ->SetCullMode(RHI::ECullMode::Disable)
            ->Confirm();

        SkyboxPipeline = RHI::FRenderPipeline::Create(PipelineLayout, PipelineSetting,
            FShader::Create("URP-Background-Skybox.slang", "VertexMain")->Compile()->GetCompiledShader(),
            FShader::Create("URP-Background-Skybox.slang", "FragmentMain")->Compile()->GetCompiledShader());

        AddSubpass(FSubpass{
				.Pipeline = SkyboxPipeline,

				.ColorImageReferences = {RHI::SV_Color},

				.SrcStage = RHI::EGraphicsPipelineStage::PipelineTop,
				.SrcStageAccess = RHI::EAccess::None,
				.DstStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
				.DstStageAccess = RHI::EAccess::W_ColorAttachment,});

    }

} // namespace VE