module;
#include <Visera.h>
#include <imgui.h>
export module Visera.Editor.Widget:Canvas;
import :Interface;

import Visera.Core.Media.Image;
import Visera.Runtime.RHI;

export namespace VE
{

  class FCanvas : public IWidget
  {
    static inline UInt32 ID = 0;
  public:
    virtual void Render() const override
    {
      if (! IsVisible()) { return; }

      ImGui::Begin(Title.c_str());
      ImGui::Image(ImTextureID(DescriptorSet->GetHandle()),
          Extent,
          {0,1},
          {1, 0}); // UV Flipped.
      ImGui::End();
    }

    FCanvas() = delete;
    FCanvas(SharedPtr<const FImage> _Image, SharedPtr<const RHI::FSampler> _Sampler, SharedPtr<const RHI::FDescriptorSetLayout> _ImGuiDSLayout);

  private:
    String  Title;
    ImVec2  Extent;
    WeakPtr<const FImage>          ImageReference;
    SharedPtr<RHI::FImage>         RHIImage;
    SharedPtr<RHI::FImageView>     RHIImageView;
    SharedPtr<RHI::FDescriptorSet> DescriptorSet;
  };

  FCanvas::
  FCanvas(SharedPtr<const FImage> _Image, SharedPtr<const RHI::FSampler> _Sampler, SharedPtr<const RHI::FDescriptorSetLayout> _ImGuiDSLayout)
    : IWidget{ FName{Text("editor::widget::canvas_{}", ID++)} },
      Title  { GetName().GetNameWithNumber() },
      Extent { Float(_Image->GetWidth()), Float(_Image->GetHeight()) },
      ImageReference {_Image}
  {
    RHIImage = RHI::CreateImage(
        RHI::EImageType::Image2D,
        RHI::FExtent3D{_Image->GetWidth(), _Image->GetHeight(), 1},
        _Image->IsSRGB()? RHI::EFormat::U32_sRGB_B8_G8_R8_A8 : RHI::EFormat::U32_Normalized_B8_G8_R8_A8,
        RHI::EImageAspect::Color,
        RHI::EImageUsage::Sampled | RHI::EImageUsage::TransferDestination);

    auto StagingBuffer = RHI::CreateStagingBuffer(_Image->GetSize());
    StagingBuffer->Write(_Image->GetData(), _Image->GetSize());

    auto Fence = RHI::CreateFence();
    auto ImmeCmd = RHI::CreateImmediateCommandBuffer();
    ImmeCmd->StartRecording();
    {
      ImmeCmd->ConvertImageLayout(RHIImage, RHI::EImageLayout::TransferDestination);
      ImmeCmd->WriteImage(RHIImage, StagingBuffer);
      ImmeCmd->ConvertImageLayout(RHIImage, RHI::EImageLayout::ShaderReadOnly);
    }
    ImmeCmd->StopRecording();
    ImmeCmd->Submit(RHI::FCommandSubmitInfo
                    {
                      .WaitStages = RHI::EGraphicsPipelineStage::PipelineTop,
                      .SignalFence = &Fence,
                    });
    Fence.Wait();

    RHIImageView = RHIImage->CreateImageView();
    DescriptorSet = RHI::CreateDescriptorSet(_ImGuiDSLayout);
    DescriptorSet->WriteImage(0, RHIImageView, _Sampler);
  }

} // namespace VE