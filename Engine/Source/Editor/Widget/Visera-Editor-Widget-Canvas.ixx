module;
#include <Visera.h>
#include <imgui.h>
export module Visera.Editor.Widget:Canvas;
#define VE_MODULE_NAME "Widget:Canvas"
import :Interface;

import Visera.Core.Log;
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
                RHI::FExtent3D{ _NewImage->GetWidth(), _NewImage->GetHeight(), 1},
                _NewImage->IsSRGB()? RHI::EFormat::U32_Normalized_B8_G8_R8_A8 : RHI::EFormat::U24_Normalized_B8_G8_R8,
                RHI::EImageAspect::Color,
                RHI::EImageUsage::Sampled | RHI::EImageUsage::TransferDestination);


        VE_LOG_WARN("{},{}", UInt64(NewRHIImage->GetSize()), _NewImage->GetSize());
        auto StagingBuffer = RHI::CreateStagingBuffer(NewRHIImage->GetSize());
        StagingBuffer->Write(_NewImage->GetData(), _NewImage->GetSize());

        auto ImmeCmd = RHI::CreateOneTimeGraphicsCommandBuffer();
        ImmeCmd->StartRecording();
        {
            ImmeCmd->ConvertImageLayout(NewRHIImage, RHI::EImageLayout::TransferDestination);
            ImmeCmd->WriteImage(NewRHIImage, StagingBuffer);
            ImmeCmd->ConvertImageLayout(NewRHIImage, RHI::EImageLayout::ShaderReadOnly);
        }
        ImmeCmd->StopRecording();
        
        ImmeCmd->SubmitAndWait();

        auto NewRHIImageView  = NewRHIImage->CreateImageView();
        DescriptorSet->WriteImage(0, NewRHIImageView, ImageSampler);

        RHIImage.swap(NewRHIImage);
        RHIImageView.swap(NewRHIImageView);
    }

    void FCanvas::
    Write(SharedPtr<const RHI::FImageView> _RHIImageView, SharedPtr<const RHI::FSampler> _Sampler/* = nullptr*/)
    {
        if (_RHIImageView->IsExpired())
        { VE_LOG_FATAL("Failed to write FCanvas -- RHIImage is expired."); }

        const auto& Sampler = _Sampler ? _Sampler : ImageSampler;

        if (RHI::EImageLayout::ShaderReadOnly == _RHIImageView->GetLayoutView())
        {
            DescriptorSet->WriteImage(0, _RHIImageView, Sampler);
        }
        else VE_LOG_FATAL("Failed to write FCanvas -- convert the RHIImage to EImageLayout::ShaderReadOnly before writing.");
    }

} // namespace VE