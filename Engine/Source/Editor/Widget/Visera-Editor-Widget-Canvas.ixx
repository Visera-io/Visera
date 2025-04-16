module;
#include <Visera.h>
#include <imgui.h>
export module Visera.Editor.Widget:Canvas;
import :Interface;

import Visera.Core.Signal;
import Visera.Core.Media.Image;
import Visera.Runtime.Render.RHI;

export namespace VE
{

class FCanvas : public IWidget
{
    static inline UInt32 ID = 0;
public:
    static inline auto
    Create(SharedPtr<RHI::FDescriptorSet> _DescriptorSet, SharedPtr<const RHI::FSampler> _Sampler) { return CreateSharedPtr<FCanvas>(_DescriptorSet, _Sampler);  }

    virtual void Render() const override
    {
        if (!IsVisible()) { return; }

        ImGui::Begin(Title.c_str());
        ImGui::Image(ImTextureID(DescriptorSet->GetHandle()),
          Extent,
          {0, 1},
          {1, 0}); // UV Flipped.
        ImGui::End();
    }

    void Write(SharedPtr<const FImage> _NewImage);
    void Write(SharedPtr<const RHI::FImageView> _RHIImageView, SharedPtr<const RHI::FSampler> _Sampler = nullptr);

    auto inline
    GetTexture() const -> SharedPtr<const RHI::FDescriptorSet> { return DescriptorSet; }

    FCanvas() = delete;
    FCanvas(SharedPtr<RHI::FDescriptorSet> _DescriptorSet, SharedPtr<const RHI::FSampler> _Sampler);

    private:
        String  Title;
        ImVec2  Extent{ 0, 0 };
        SharedPtr<const RHI::FSampler> ImageSampler;
        SharedPtr<RHI::FImage>         RHIImage;
        SharedPtr<RHI::FImageView>     RHIImageView;
        SharedPtr<RHI::FDescriptorSet> DescriptorSet;
    };

    FCanvas::
    FCanvas(SharedPtr<RHI::FDescriptorSet> _DescriptorSet, SharedPtr<const RHI::FSampler> _Sampler)
    : IWidget{ FName{Text("editor::widget::canvas_{}", ID++)} },
      Title     { GetName().GetNameWithNumber() },
      Extent    { 0, 0 },
      ImageSampler { std::move(_Sampler) },
      DescriptorSet{ std::move(_DescriptorSet) }
    {
      VE_ASSERT(DescriptorSet && ImageSampler);
    }

    void FCanvas::
    Write(SharedPtr<const FImage> _NewImage)
    {
        VE_ASSERT(_NewImage && _NewImage->GetSize());

        Extent = { Float(_NewImage->GetWidth()), Float(_NewImage->GetHeight()) };

        auto NewRHIImage = RHI::CreateImage(
                RHI::EImageType::Image2D,
                RHI::FExtent3D{ UInt32(Extent.x), UInt32(Extent.y), 1},
                _NewImage->IsSRGB()? RHI::EFormat::U32_sRGB_B8_G8_R8_A8 : RHI::EFormat::U32_Normalized_B8_G8_R8_A8,
                RHI::EImageAspect::Color,
                RHI::EImageUsage::Sampled | RHI::EImageUsage::TransferDestination);

        auto StagingBuffer = RHI::CreateStagingBuffer(_NewImage->GetSize());
        StagingBuffer->Write(_NewImage->GetData(), _NewImage->GetSize());

        auto Fence = RHI::CreateFence();
        auto ImmeCmd = RHI::CreateOneTimeGraphicsCommandBuffer();
        ImmeCmd->StartRecording();
        {
            ImmeCmd->ConvertImageLayout(NewRHIImage, RHI::EImageLayout::TransferDestination);
            ImmeCmd->WriteImage(NewRHIImage, StagingBuffer);
            ImmeCmd->ConvertImageLayout(NewRHIImage, RHI::EImageLayout::ShaderReadOnly);
        }
        ImmeCmd->StopRecording();
        
		RHI::EGraphicsPipelineStage WaitStages = RHI::EGraphicsPipelineStage::PipelineTop;
        ImmeCmd->Submit(RHI::FCommandSubmitInfo
                        {
                            .pWaitStages {.Graphics = &WaitStages },
                            .SignalFence = &Fence,
                        });
        Fence.Wait();

        auto NewRHIImageView  = NewRHIImage->CreateImageView();
        DescriptorSet->WriteImage(0, NewRHIImageView, ImageSampler);

        RHIImage.swap(NewRHIImage);
        RHIImageView.swap(NewRHIImageView);
    }

    void FCanvas::
    Write(SharedPtr<const RHI::FImageView> _RHIImageView, SharedPtr<const RHI::FSampler> _Sampler/* = nullptr*/)
    {
        if (_RHIImageView->IsExpired())
        { throw SRuntimeError("Failed to write FCanvas -- RHIImage is expired."); }

        const auto& Sampler = _Sampler ? _Sampler : ImageSampler;

        if (RHI::EImageLayout::ShaderReadOnly == _RHIImageView->GetLayoutView())
        {
            DescriptorSet->WriteImage(0, _RHIImageView, Sampler);
        }
        else throw SRuntimeError("Failed to write FCanvas -- convert the RHIImage to EImageLayout::ShaderReadOnly before writing.");
    }

} // namespace VE