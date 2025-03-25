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
    void Write(SharedPtr<const FImage> _NewImage, Bool _bFreeOldSet = True);

    FCanvas() = delete;
    FCanvas(SharedPtr<const FImage> _Image, SharedPtr<const RHI::FSampler> _Sampler, SharedPtr<const RHI::FDescriptorSetLayout> _ImGuiDSLayout);

  private:
    String  Title;
    ImVec2  Extent;
    WeakPtr<const FImage>          ImageReference;
    SharedPtr<const RHI::FSampler> Sampler;
    SharedPtr<RHI::FImage>         RHIImage;
    SharedPtr<RHI::FImageView>     RHIImageView;
    SharedPtr<RHI::FDescriptorSet> DescriptorSet;
    SharedPtr<const RHI::FDescriptorSetLayout> DescriptorSetLayout;
  };

  FCanvas::
  FCanvas(SharedPtr<const FImage> _Image, SharedPtr<const RHI::FSampler> _Sampler, SharedPtr<const RHI::FDescriptorSetLayout> _ImGuiDSLayout)
    : IWidget{ FName{Text("editor::widget::canvas_{}", ID++)} },
      Title  { GetName().GetNameWithNumber() },
      Extent { Float(_Image->GetWidth()), Float(_Image->GetHeight()) },
      ImageReference {_Image},
      Sampler{std::move(_Sampler)},
      DescriptorSet{ RHI::CreateDescriptorSet(_ImGuiDSLayout) },
      DescriptorSetLayout{ _ImGuiDSLayout }
  {
      Write(_Image, False);
  }

  void FCanvas::
  Write(SharedPtr<const FImage> _NewImage, Bool _bFreeOldSet /* = True*/)
  {
    if (_bFreeOldSet)
    {
      RHI::WaitDeviceIdle(); //[TODO]: Fixme
      RHI::FreeDescriptorSet(DescriptorSet);
      DescriptorSet = RHI::CreateDescriptorSet(DescriptorSetLayout);
    }

    RHIImageView.reset();
    RHIImage = RHI::CreateImage(
          RHI::EImageType::Image2D,
          RHI::FExtent3D{_NewImage->GetWidth(), _NewImage->GetHeight(), 1},
          _NewImage->IsSRGB()? RHI::EFormat::U32_sRGB_B8_G8_R8_A8 : RHI::EFormat::U32_Normalized_B8_G8_R8_A8,
          RHI::EImageAspect::Color,
          RHI::EImageUsage::Sampled | RHI::EImageUsage::TransferDestination);

    auto StagingBuffer = RHI::CreateStagingBuffer(_NewImage->GetSize());
    StagingBuffer->Write(_NewImage->GetData(), _NewImage->GetSize());

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

    RHIImageView  = RHIImage->CreateImageView();
    DescriptorSet->WriteImage(0, RHIImageView, Sampler);
  }

} // namespace VE