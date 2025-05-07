module;
#include <Visera.h>
export module Visera.Runtime.Render.Light;
#define VE_MODULE_NAME "Light"

import Visera.Core.Log;
import Visera.Core.Math.Basic;
import Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.Texture;
import Visera.Runtime.Render.Camera;

export namespace VE
{

    class FLight
    {
    public:
        struct FUBOLayout
        {
            alignas(16) Vector3F   Position;
            alignas(16) Matrix4x4F LightViewing = Matrix4x4F::Identity();
        };

        void inline
        Render(SharedPtr<RHI::FGraphicsCommandBuffer> _CommandBuffer, SharedPtr<const RHI::FRenderPass> _ShadowPass);

        FLight();

    private:
        FCamera                        Camera;
        Array<SharedPtr<FShadowMap>>   ShadowMaps;
        Array<SharedPtr<RHI::FBuffer>> UBOBuffers;
        Array<SharedPtr<RHI::FDescriptorSet>> UBOs;
    };

    void FLight::
    Render(SharedPtr<RHI::FGraphicsCommandBuffer> _CommandBuffer, SharedPtr<const RHI::FRenderPass> _ShadowPass)
    {
        VE_ASSERT(_ShadowPass->GetType() == RHI::FRenderPass::EType::Shadow);
        _CommandBuffer->ReachRenderPass(_ShadowPass);

        UInt32 FrameIdx = RHI::GetSwapchainCursor();
        ShadowMaps[FrameIdx]->ReachShadowPass(_CommandBuffer);
        {
            _CommandBuffer->BindDescriptorSet(0, UBOs[FrameIdx]);
        }
        ShadowMaps[FrameIdx]->LeaveShadowPass(_CommandBuffer);
    }

    FLight::
    FLight()
    {
        ShadowMaps = FShadowMap::BatchCreate(
            RHI::GetSwapchainFrameCount(),
            RHI::FFrameContext::RenderArea.extent.width,
            RHI::FFrameContext::RenderArea.extent.height);

        UBOs.resize(RHI::GetSwapchainFrameCount());
        UBOBuffers.resize(UBOs.size());
        for (UInt32 Idx = 0; Idx < UBOs.size(); ++Idx)
        {
            UBOs[Idx] = RHI::CreateDescriptorSet(RHI::FFrameContext::LightUBODSLayout);
            UBOBuffers[Idx] = RHI::CreateBuffer(sizeof(FUBOLayout), RHI::EBufferUsage::Uniform);
        }

        VE_LOG_INFO("Created a new light (position:[{}, {}, {}]).",
            Camera.GetPosition().x(), Camera.GetPosition().y(), Camera.GetPosition().z());
    }

} // namespace VE