module;
#include <Visera.h>
#include <imgui.h>
export module Visera.Editor.Widget:Canvas;
#define VE_MODULE_NAME "Widget:Canvas"
import :Interface;

import Visera.Core.Log;
import Visera.Core.Media.Image;
import Visera.Runtime.Render.RHI;
import Visera.Runtime.Render.Texture;

export namespace VE
{

class FCanvas : public IWidget
{
    static inline UInt32 ID = 0;
public:
    static inline auto
    Create(SharedPtr<RHI::FDescriptorSet> _DescriptorSet)
    { return CreateSharedPtr<FCanvas>(_DescriptorSet);  }

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
    FCanvas(SharedPtr<RHI::FDescriptorSet> _DescriptorSet);

    private:
        String  Title;
        ImVec2  Extent{ 0, 0 };
        SharedPtr<FSRGBTexture2D>          Texture;
        SharedPtr<RHI::FDescriptorSet> DescriptorSet;
    };

    FCanvas::
    FCanvas(SharedPtr<RHI::FDescriptorSet> _DescriptorSet)
    : IWidget{ FName{Text("editor::widget::canvas_{}", ID++)} },
      Title     { GetName().GetNameWithNumber() },
      Extent    { 0, 0 },
      DescriptorSet{ std::move(_DescriptorSet) }
    {
      VE_ASSERT(DescriptorSet);
    }

    void FCanvas::
    Write(SharedPtr<const FImage> _NewImage)
    {
        VE_ASSERT(_NewImage && _NewImage->GetSize());

        Extent = { Float(_NewImage->GetWidth()), Float(_NewImage->GetHeight()) };

        auto NewTexture = FSRGBTexture2D::Create(_NewImage);
        NewTexture->BindToDescriptorSet(DescriptorSet, 0);
        Texture.swap(NewTexture);
    }

    void FCanvas::
    Write(SharedPtr<const RHI::FImageView> _RHIImageView, SharedPtr<const RHI::FSampler> _Sampler/* = nullptr*/)
    {
        if (_RHIImageView->IsExpired())
        { VE_LOG_FATAL("Failed to write FCanvas -- RHIImage is expired."); }

        const auto Sampler = _Sampler ? _Sampler : RHI::GetGlobalTexture2DSampler();

        if (RHI::EImageLayout::ShaderReadOnly == _RHIImageView->GetLayoutView())
        {
            DescriptorSet->WriteImage(0, _RHIImageView, Sampler);
        }
        else VE_LOG_FATAL("Failed to write FCanvas -- convert the RHIImage to EImageLayout::ShaderReadOnly before writing.");
    }

} // namespace VE