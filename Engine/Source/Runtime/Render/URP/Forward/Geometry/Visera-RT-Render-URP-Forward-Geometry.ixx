module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Forward.Geometry;
import :Opaque;

import Visera.Core.OS.FileSystem;
import Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.Scene.Primitive;

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
                    },
                    {
                        .Location = 1,
                        .Format   = RHI::EFormat::Vertex3F,
                        .Offset   = offsetof(FMeshPrimitive::FVertex, Normal),
                    }
                }
            });

        auto VertSPIRV = FileSystem::CreateBinaryFile(FPath{ VISERA_APP_SHADERS_DIR"/opaque.vert.spv" });
		VertSPIRV->Load();
		auto FragSPIRV = FileSystem::CreateBinaryFile(FPath{ VISERA_APP_SHADERS_DIR"/opaque.frag.spv" });
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