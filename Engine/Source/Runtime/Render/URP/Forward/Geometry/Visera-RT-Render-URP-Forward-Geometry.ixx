module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Forward.Geometry;
import :Opaque;

import Visera.Core.OS.FileSystem;
import Visera.Runtime.Render.RHI;

export namespace VE
{
    
    class FURPGeometryPass : public RHI::FRenderPass
    {
    public:
        FURPGeometryPass();

        auto GetOpaquePipeline() const -> SharedPtr<const FURPOpaquePipeline> { return OpaquePipeline; }

    private:
        SharedPtr<RHI::FPipelineLayout>        PipelineLayout;
        SharedPtr<RHI::FRenderPipelineSetting> PipelineSetting;

        SharedPtr<FURPOpaquePipeline>          OpaquePipeline;
    };

    FURPGeometryPass::
    FURPGeometryPass() : RHI::FRenderPass{ EType::DefaultForward }
    {
        PipelineLayout  = RHI::CreatePipelineLayout();
        PipelineSetting = RHI::CreateRenderPipelineSetting();

        auto VertSPIRV = FileSystem::CreateBinaryFile(FPath{ VISERA_APP_SHADERS_DIR"/test.vert.spv" });
		VertSPIRV->Load();
		auto FragSPIRV = FileSystem::CreateBinaryFile(FPath{ VISERA_APP_SHADERS_DIR"/test.frag.spv" });
		FragSPIRV->Load();

        OpaquePipeline = CreateSharedPtr<FURPOpaquePipeline>(PipelineLayout, PipelineSetting,
            RHI::CreateShader(RHI::EShaderStage::Vertex,   VertSPIRV->GetRawData(), VertSPIRV->GetSize()),
            RHI::CreateShader(RHI::EShaderStage::Fragment, FragSPIRV->GetRawData(), FragSPIRV->GetSize()));

        AddSubpass(FSubpass{
				.Pipeline = OpaquePipeline,

				.ColorImageReferences = {RHI::SV_Color},

				.SrcStage = RHI::EGraphicsPipelineStage::PipelineTop,
				.SrcStageAccess = RHI::EAccess::None,
				.DstStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
				.DstStageAccess = RHI::EAccess::W_ColorAttachment,});

    }

} // namespace VE