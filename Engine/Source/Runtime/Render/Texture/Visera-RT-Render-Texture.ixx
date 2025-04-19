module;
#include <Visera.h>
export module Visera.Runtime.Render.Texture;
#define VE_MODULE_NAME "Texture"
export import :SRGB;

import Visera.Core.Log;
import Visera.Core.Media.Image;
import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FTexture2D
    {
    public:
        static inline auto
        Create(SharedPtr<const IImage> _Image, SharedPtr<const RHI::FSampler> _Sampler = nullptr)
        { return CreateSharedPtr<FTexture2D>(_Image, _Sampler); }

        void inline
        BindToDescriptorSet(SharedPtr<RHI::FDescriptorSet> _DescriptorSet, UInt32 _Binding) const;

        FTexture2D() = delete;
        FTexture2D(SharedPtr<const IImage> _Image, SharedPtr<const RHI::FSampler> _Sampler = nullptr);
        ~FTexture2D() = default;

    private:
        WeakPtr<const IImage>          ImageReference;
        SharedPtr<RHI::FImage>         RHIImage;
        SharedPtr<RHI::FImageView>     RHIImageView;
        SharedPtr<const RHI::FSampler> Sampler;

    private:
        struct FSupportedFormat
        {
            const char*   Name;
            RHI::EFormat  Format;
            RHI::FSwizzle Swizzle{};
        };
        static auto SearchSupportedFormat(SharedPtr<const IImage> _Image) -> FSupportedFormat;
    };

    void FTexture2D::
    BindToDescriptorSet(SharedPtr<RHI::FDescriptorSet> _DescriptorSet, UInt32 _Binding) const
    {
        _DescriptorSet->WriteImage(_Binding, RHIImageView, Sampler);
    }

    FTexture2D::
    FTexture2D(SharedPtr<const IImage> _Image, SharedPtr<const RHI::FSampler> _Sampler/* = nullptr */)
        :ImageReference{_Image},
         Sampler{_Sampler? _Sampler : RHI::GetGlobalTexture2DSampler()}
    {
        VE_ASSERT(Sampler->IsBuilt());
        //Check hardware supports and convert to valid format.
        if (_Image->IsType(EImageType::Bitmap))
        {
            VE_LOG_ERROR("Currently NOT support HDR/PFM/TIFF/GrayScale...!");
            return;
        }

        const auto SupportedFormat = SearchSupportedFormat(_Image);

        RHIImage = RHI::CreateImage(RHI::EImageType::Image2D,
                                    {.width = _Image->GetWidth(), .height = _Image->GetHeight(), .depth = 1},
                                    SupportedFormat.Format,
                                    RHI::EImageAspect::Color,
                                    RHI::EImageUsage::Sampled | RHI::EImageUsage::TransferDestination,
                                    SupportedFormat.Swizzle);

        auto StagingBuffer = RHI::CreateStagingBuffer(RHIImage->GetSize());
        StagingBuffer->Write(_Image->GetData(), _Image->GetMemorySize());

        auto ImmeCmd = RHI::CreateOneTimeGraphicsCommandBuffer();
        ImmeCmd->StartRecording();
        {
            ImmeCmd->ConvertImageLayout(RHIImage, RHI::EImageLayout::TransferDestination);
            ImmeCmd->WriteImage(RHIImage, StagingBuffer);
            ImmeCmd->ConvertImageLayout(RHIImage, RHI::EImageLayout::ShaderReadOnly);
        }
        ImmeCmd->StopRecording();

        ImmeCmd->SubmitAndWait();

        RHIImageView = RHIImage->CreateImageView();

        VE_LOG_INFO("Created a new Texture2D (format:{}, sampler:{}).", SupportedFormat.Name, (Address)(Sampler->GetHandle()));
    }

    FTexture2D::FSupportedFormat FTexture2D::
    SearchSupportedFormat(SharedPtr<const IImage> _Image)
    {
        FSupportedFormat SupportedFormat{};

        if (_Image->IsSRGB())
        {
            switch (_Image->GetFormat())
            {
            case EImageFormat::RGB_R8_G8_B8:
                if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_sRGB_R8_G8_B8))
                {
                    SupportedFormat.Name   = "U24_Normalized_R8_G8_B8";
                    SupportedFormat.Format = RHI::EFormat::U24_sRGB_R8_G8_B8;
                    return SupportedFormat;
                }
            case EImageFormat::RGB_B8_G8_R8:
                if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_sRGB_B8_G8_R8))
                {
                    SupportedFormat.Name      = "U24_sRGB_B8_G8_R8";
                    SupportedFormat.Format    = RHI::EFormat::U24_sRGB_B8_G8_R8;
                    SupportedFormat.Swizzle.R = RHI::ESwizzle::B;
                    SupportedFormat.Swizzle.B = RHI::ESwizzle::R;
                    return SupportedFormat;
                }
           default:
                VE_LOG_WARN("Current platform do NOT support sRGB image format, the image was forced to be converted to sRGBA!");
                std::const_pointer_cast<IImage>(_Image)->ConvertToRGBA();
                //Pass to the next branch
            }

            switch (_Image->GetFormat())
            {
            case EImageFormat::RGBA_R8_G8_B8_A8:
                if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_sRGB_R8_G8_B8_A8))
                {
                    SupportedFormat.Name   = "U32_sRGB_R8_G8_B8_A8";
                    SupportedFormat.Format = RHI::EFormat::U32_sRGB_R8_G8_B8_A8;
                    return SupportedFormat;
                }
            case EImageFormat::RGBA_B8_G8_R8_A8:
                if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_sRGB_B8_G8_R8_A8))
                {
                    SupportedFormat.Name      = "U32_sRGB_B8_G8_R8_A8";
                    SupportedFormat.Format    = RHI::EFormat::U32_sRGB_B8_G8_R8_A8;
                    SupportedFormat.Swizzle.R = RHI::ESwizzle::B;
                    SupportedFormat.Swizzle.B = RHI::ESwizzle::R;
                    return SupportedFormat;
                }
            default:
                VE_LOG_ERROR("Failed to find a proper texture format for the image ({})!", _Image->GetInfo());
            }

            return SupportedFormat;
        }

        //Linear Image (Non-sRGB)
        switch (_Image->GetFormat())
        {
        case EImageFormat::RGB_R8_G8_B8:
            if(RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_Normalized_R8_G8_B8))
            {
                SupportedFormat.Name   = "U24_Normalized_R8_G8_B8";
                SupportedFormat.Format = RHI::EFormat::U24_Normalized_R8_G8_B8;
                return SupportedFormat;
            }
        case EImageFormat::RGB_B8_G8_R8:
            if(RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_Normalized_B8_G8_R8))
            {
                SupportedFormat.Name      = "U24_Normalized_B8_G8_R8";
                SupportedFormat.Format    = RHI::EFormat::U24_Normalized_B8_G8_R8;
                SupportedFormat.Swizzle.R = RHI::ESwizzle::B;
                SupportedFormat.Swizzle.B = RHI::ESwizzle::R;
                return SupportedFormat;
            }
        default:
            VE_LOG_WARN("Current platform do NOT support RGB image format, the image was forced to be converted to RGBA!");
            std::const_pointer_cast<IImage>(_Image)->ConvertToRGBA();
            //Pass to the next branch
        }

        switch (_Image->GetFormat())
        {
        case EImageFormat::RGBA_R8_G8_B8_A8:
            if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_Normalized_R8_G8_B8_A8))
            {
                SupportedFormat.Name   = "U32_Normalized_R8_G8_B8_A8";
                SupportedFormat.Format = RHI::EFormat::U32_Normalized_R8_G8_B8_A8;
                return SupportedFormat;
            }
        case EImageFormat::RGBA_B8_G8_R8_A8:
            if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_Normalized_B8_G8_R8_A8))
            {
                SupportedFormat.Name      = "U32_Normalized_B8_G8_R8_A8";
                SupportedFormat.Format    = RHI::EFormat::U32_Normalized_B8_G8_R8_A8;
                SupportedFormat.Swizzle.R = RHI::ESwizzle::B;
                SupportedFormat.Swizzle.B = RHI::ESwizzle::R;
                return SupportedFormat;
            }
        default:
            VE_LOG_ERROR("Failed to find a proper texture format for the image ({})!", _Image->GetInfo());
        }
        return SupportedFormat;
    }

} // namespace VE