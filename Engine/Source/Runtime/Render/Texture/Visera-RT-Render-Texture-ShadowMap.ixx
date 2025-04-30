module;
#include <Visera.h>
export module Visera.Runtime.Render.Texture:ShadowMap;
#define VE_MODULE_NAME "Texture:ShadowMap"
import :Interface;

import Visera.Core.Log;
import Visera.Core.Media.Image;
import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FShadowMap : public ITexture
    {
    public:
        static inline auto
        Create(UInt32 _Width, UInt32 _Height)
        { return CreateSharedPtr<FShadowMap>(_Width, _Height); }

        static inline Array<SharedPtr<FShadowMap>>
        BatchCreate(UInt32 _Count, UInt32 _Width, UInt32 _Height);

        FShadowMap() = default;
        FShadowMap(UInt32 _Width, UInt32 _Height);
        virtual ~FShadowMap() override = default;


    private:
        static inline Optional<FSupportedFormat> SupportedFormat;
        static inline FSupportedFormat SearchSupportedFormat();
    };

    FShadowMap::
    FShadowMap(UInt32 _Width, UInt32 _Height) : ITexture{_Width, _Height}
    {
        const auto SupportedFormat = SearchSupportedFormat();

        RHIImage = RHI::CreateImage(RHI::EImageType::Image2D,
                                    {.width = _Width, .height = _Height, .depth = 1},
                                    SupportedFormat.Format,
                                    RHI::EImageAspect::Depth,
                                    RHI::EImageUsage::Sampled | RHI::EImageUsage::DepthStencilAttachment,
                                    SupportedFormat.Swizzle);

        auto ImmeCmd = RHI::CreateOneTimeGraphicsCommandBuffer();
        ImmeCmd->StartRecording();
        {
            ImmeCmd->ConvertImageLayout(RHIImage, RHI::EImageLayout::DepthAttachment);
        }
        ImmeCmd->StopRecording();

        ImmeCmd->SubmitAndWait();

        RHIImageView = RHIImage->CreateImageView();

        VE_LOG_INFO("Created a Shadow Map (extent:[W{}, H{}]).", _Width, _Height);
    }

    Array<SharedPtr<FShadowMap>> FShadowMap::
    BatchCreate(UInt32 _Count, UInt32 _Width, UInt32 _Height)
    {
        static const auto SupportInfo = SearchSupportedFormat();

        VE_ASSERT(_Count > 0);
        Array<SharedPtr<FShadowMap>> ShadowMaps(_Count);

        auto ImmeCmd = RHI::CreateOneTimeGraphicsCommandBuffer();
        ImmeCmd->StartRecording();
        for (auto& ShadowMap : ShadowMaps)
        {
            ShadowMap = CreateSharedPtr<FShadowMap>();
            ShadowMap->RHIImage = RHI::CreateImage(RHI::EImageType::Image2D,
                                {.width = _Width, .height = _Height, .depth = 1},
                                SupportInfo.Format,
                                RHI::EImageAspect::Depth,
                                RHI::EImageUsage::Sampled | RHI::EImageUsage::DepthStencilAttachment,
                                SupportInfo.Swizzle);

            ImmeCmd->ConvertImageLayout(ShadowMap->RHIImage, RHI::EImageLayout::DepthAttachment);
            ShadowMap->RHIImageView = ShadowMap->RHIImage->CreateImageView();
        }
        ImmeCmd->StopRecording();
        ImmeCmd->SubmitAndWait();

        VE_LOG_INFO("Created {} Shadow Maps (extent:[W{}, H{}]).", _Count, _Width, _Height);
        return ShadowMaps;
    }

    FShadowMap::FSupportedFormat FShadowMap::
    SearchSupportedFormat()
    {
        if (SupportedFormat.has_value())
        { return SupportedFormat.value(); }

        if (RHI::IsTexture2DFormatSupported(RHI::EFormat::S32_Float_Depth32))
        {
            SupportedFormat = FSupportedFormat
            {
                .Name   = "S32_Float_Depth32",
                .Format = RHI::EFormat::S32_Float_Depth32,
            };
        }
        else
        {
            VE_LOG_FATAL("Cannot find a supported ShadowMap format!");
        }

        return SupportedFormat.value();
    }

} // namespace VE