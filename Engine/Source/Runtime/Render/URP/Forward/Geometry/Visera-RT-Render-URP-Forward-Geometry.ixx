module;
#include <Visera.h>
export module Visera.Runtime.Render.URP.Forward.Geometry;
import :Opaque;

import Visera.Core.OS.FileSystem;

import Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.Shader;
import Visera.Runtime.Render.Scene.Primitive;

export namespace VE
{
    
    class FURPGeometryPass : public RHI::FRenderPass
    {
    public:
        FURPGeometryPass();
        auto GetOpaquePipeline() const -> SharedPtr<const RHI::FRenderPipeline> { return OpaquePipeline; }

    private:
        //Resources
        SharedPtr<RHI::FDescriptorSetLayout>   TransformsDSLayout;
        SharedPtr<RHI::FDescriptorSetLayout>   TexturesDSLayout;
        //Layouts
        SharedPtr<RHI::FPipelineLayout>        PipelineLayout;
        SharedPtr<RHI::FRenderPipelineSetting> PipelineSetting;
        //Pipelines
        SharedPtr<RHI::FRenderPipeline>        OpaquePipeline;
    };

    FURPGeometryPass::
    FURPGeometryPass() : RHI::FRenderPass{ EType::DefaultForward }
    {
        TransformsDSLayout = RHI::CreateDescriptorSetLayout()
            ->AddBinding(0, RHI::EDescriptorType::UniformBuffer, 1, RHI::EShaderStage::Vertex)
            ->Build();

        TexturesDSLayout = RHI::CreateDescriptorSetLayout()
            ->AddBinding(0, RHI::EDescriptorType::CombinedImageSampler, 1, RHI::EShaderStage::Fragment)
            ->Build();

        PipelineLayout = RHI::CreatePipelineLayout()
            ->AddPushConstantRange(0, sizeof(UInt32), RHI::EShaderStage::Vertex | RHI::EShaderStage::Fragment)
            ->AddDescriptorSetLayout(TransformsDSLayout)
            ->AddDescriptorSetLayout(TexturesDSLayout)
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
                    },
                    {
                        .Location = 1,
                        .Format   = RHI::EFormat::Vertex3F,
                        .Offset   = offsetof(FMeshPrimitive::FVertex, Normal),
                    },
                    {
                        .Location = 2,
                        .Format   = RHI::EFormat::Vertex3F,
                        .Offset   = offsetof(FMeshPrimitive::FVertex, TextureCoord),
                    }
                }
            });

        OpaquePipeline = RHI::FRenderPipeline::Create(PipelineLayout, PipelineSetting,
            FShader::Create("URP-Forward-Geometry.slang", "VertexMain")->Compile()->GetCompiledShader(),
            FShader::Create("URP-Forward-Geometry.slang", "FragmentMain")->Compile()->GetCompiledShader());

        AddSubpass(FSubpass{
				.Pipeline = OpaquePipeline,

				.ColorImageReferences = {RHI::SV_Color},

				.SrcStage = RHI::EGraphicsPipelineStage::PipelineTop,
				.SrcStageAccess = RHI::EAccess::None,
				.DstStage = RHI::EGraphicsPipelineStage::ColorAttachmentOutput,
				.DstStageAccess = RHI::EAccess::W_ColorAttachment,});
    }

} // namespace VE