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
        Create(SharedPtr<const FImage> _Image, SharedPtr<const RHI::FSampler> _Sampler = nullptr)
        { return CreateSharedPtr<FTexture2D>(_Image, _Sampler); }

        void inline
        BindToDescriptorSet(SharedPtr<RHI::FDescriptorSet> _DescriptorSet, UInt32 _Binding) const;

        FTexture2D() = delete;
        FTexture2D(SharedPtr<const FImage> _Image, SharedPtr<const RHI::FSampler> _Sampler = nullptr);
        ~FTexture2D();

    private:
        WeakPtr<const FImage>          ImageReference;
        SharedPtr<RHI::FImage>         RHIImage;
        SharedPtr<RHI::FImageView>     RHIImageView;
        SharedPtr<const RHI::FSampler> Sampler;
    };

    void FTexture2D::
    BindToDescriptorSet(SharedPtr<RHI::FDescriptorSet> _DescriptorSet, UInt32 _Binding) const
    {
        _DescriptorSet->WriteImage(_Binding, RHIImageView, Sampler);
    }

    FTexture2D::
    FTexture2D(SharedPtr<const FImage> _Image, SharedPtr<const RHI::FSampler> _Sampler/* = nullptr */)
        :ImageReference{_Image},
         Sampler{_Sampler? _Sampler : RHI::GetGlobalTexture2DSampler()}
    {
        VE_ASSERT(Sampler->IsBuilt());
        //Check hardware supports and convert to valid format.
        if (!_Image->IsBitmap())
        {
            VE_LOG_ERROR("Currently NOT support HDR/PFM/TIFF/GrayScale...!");
            return;
        }

        // Note: FreeImage's Bitmaps are BGRA order by default, so we handle this order first.
        RHI::FSwizzle ImageSwizzle{};
        RHI::EFormat RHIImageFormat{RHI::EFormat::None};
        const char* FormatName{};

        if (_Image->IsSRGB()) switch (_Image->GetColorType())
        {
            case FImage::EColorType::RGB:
            {
                if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_sRGB_B8_G8_R8))
                {
                    FormatName = "U24_sRGB_B8_G8_R8";
                    RHIImageFormat = RHI::EFormat::U24_sRGB_B8_G8_R8;
                    break;
                }
                if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_sRGB_R8_G8_B8))
                {
                    FormatName = "U24_Normalized_R8_G8_B8";
                    RHIImageFormat = RHI::EFormat::U24_sRGB_R8_G8_B8;
                    ImageSwizzle.R = RHI::ESwizzle::B;
                    ImageSwizzle.B = RHI::ESwizzle::R;
                    break;
                }
                VE_LOG_WARN("Current platform do NOT support RGB image format, the image was forced to be converted to RGBA!");
                std::const_pointer_cast<FImage>(_Image)->ConvertToRGBA32();
                //Pass to the next branch
            }
            case FImage::EColorType::RGBA:
            {
                    if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_sRGB_B8_G8_R8_A8))
                    {
                        FormatName = "U32_sRGB_B8_G8_R8_A8";
                        RHIImageFormat = RHI::EFormat::U32_sRGB_B8_G8_R8_A8;
                        break;
                    }
                    if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_sRGB_R8_G8_B8_A8))
                    {
                        FormatName = "U32_sRGB_R8_G8_B8_A8";
                        RHIImageFormat = RHI::EFormat::U32_sRGB_R8_G8_B8_A8;
                        ImageSwizzle.R = RHI::ESwizzle::B;
                        ImageSwizzle.B = RHI::ESwizzle::R;
                        break;
                    }
            }
        }

        if (RHIImageFormat == RHI::EFormat::None && _Image->IsSRGB())
        {
            VE_LOG_WARN("Current platform do NOT support sRGB image type, the image was forced to linear color space!");
            std::const_pointer_cast<FImage>(_Image)->ConvertToLinear();
        }

        if (RHIImageFormat == RHI::EFormat::None)
        switch (_Image->GetColorType())
        {
        case FImage::EColorType::RGB:
        {
            if(RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_Normalized_B8_G8_R8))
            {
                FormatName = "U24_Normalized_B8_G8_R8";
                RHIImageFormat = RHI::EFormat::U24_Normalized_B8_G8_R8;
                break;
            }
            if(RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_Normalized_R8_G8_B8))
            {
                FormatName = "U24_Normalized_R8_G8_B8";
                RHIImageFormat = RHI::EFormat::U24_Normalized_R8_G8_B8;
                ImageSwizzle.R = RHI::ESwizzle::B;
                ImageSwizzle.B = RHI::ESwizzle::R;
                break;
            }
            VE_LOG_WARN("Current platform do NOT support RGB image format, the image was forced to be converted to RGBA!");
            std::const_pointer_cast<FImage>(_Image)->ConvertToRGBA32();
            //Pass to the next branch
        }
        case FImage::EColorType::RGBA:
        {
            if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_Normalized_B8_G8_R8_A8))
            {
                FormatName = "U32_Normalized_B8_G8_R8_A8";
                RHIImageFormat = RHI::EFormat::U32_Normalized_B8_G8_R8_A8;
                break;
            }
            if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_Normalized_R8_G8_B8_A8))
            {
                FormatName = "U32_Normalized_R8_G8_B8_A8";
                RHIImageFormat = RHI::EFormat::U32_Normalized_R8_G8_B8_A8;
                ImageSwizzle.R = RHI::ESwizzle::B;
                ImageSwizzle.B = RHI::ESwizzle::R;
                break;
            }
            VE_LOG_ERROR("Failed to find a suitable format for the image ({})!", _Image->GetPath().ToPlatformString());
            break;
        }
        default:
            VE_LOG_ERROR("Unsupported Texture2D format ({}) of the image ({})!", UInt32(_Image->GetFormat()), _Image->GetPath().ToPlatformString());
        }

        RHIImage = RHI::CreateImage(RHI::EImageType::Image2D,
                                    {.width = _Image->GetWidth(), .height = _Image->GetHeight(), .depth = 1},
                                    RHIImageFormat,
                                    RHI::EImageAspect::Color,
                                    RHI::EImageUsage::Sampled | RHI::EImageUsage::TransferDestination,
                                    ImageSwizzle);

        auto StagingBuffer = RHI::CreateStagingBuffer(RHIImage->GetSize());
        StagingBuffer->Write(_Image->GetData(), _Image->GetSize());

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

        VE_LOG_INFO("Created a new Texture2D (format:{}, sampler:{}).", FormatName, (Address)(Sampler->GetHandle()));
    }

    FTexture2D::
    ~FTexture2D()
    {

    }

} // namespace VE