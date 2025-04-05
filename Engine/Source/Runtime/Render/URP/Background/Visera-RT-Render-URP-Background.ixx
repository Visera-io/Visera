module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Background;
import :Skybox;

import Visera.Core.OS.FileSystem;
import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FURPBackgroundPass : public RHI::FRenderPass
    {
    public:
        FURPBackgroundPass();

        auto GetSkyboxPipeline() const -> SharedPtr<const FURPSkyboxPipeline> { return SkyboxPipeline; }

    private:
        SharedPtr<RHI::FPipelineLayout>        PipelineLayout;
        SharedPtr<RHI::FRenderPipelineSetting> PipelineSetting;

        SharedPtr<FURPSkyboxPipeline>          SkyboxPipeline;
    };

    FURPBackgroundPass::
    FURPBackgroundPass() : RHI::FRenderPass{ EType::Background }
    {
        PipelineLayout  = RHI::CreatePipelineLayout()
            ->Build();
        PipelineSetting = RHI::CreateRenderPipelineSetting();

        auto VertSPIRV = FileSystem::CreateBinaryFile(FPath{ VISERA_APP_SHADERS_DIR"/test.vert.spv" });
		VertSPIRV->Load();
		auto FragSPIRV = FileSystem::CreateBinaryFile(FPath{ VISERA_APP_SHADERS_DIR"/test.frag.spv" });
		FragSPIRV->Load();

        SkyboxPipeline = CreateSharedPtr<FURPSkyboxPipeline>(PipelineLayout, PipelineSetting,
            RHI::CreateShader(RHI::EShaderStage::Vertex,   VertSPIRV->GetRawData(), VertSPIRV->GetSize()),
            RHI::CreateShader(RHI::EShaderStage::Fragment, FragSPIRV->GetRawData(), FragSPIRV->GetSize()));

        AddSubpass(FSubpass{
				.Pipeline = SkyboxPipeline,

				.ColorImageReferences = {RHI::SV_Color},

				.SrcStage = RHI::EGraphicsPipelineStage::PipelineTop,
				.SrcStageAccess = RHI::EAccess::None,
				.DstStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
				.DstStageAccess = RHI::EAccess::W_ColorAttachment,});

    }

} // namespace VE