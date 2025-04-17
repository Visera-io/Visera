module;
#include <Visera.h>
export module Visera.Runtime.Render.Scene.Texture;
#define VE_MODULE_NAME "Texture"

import Visera.Core.Log;
import Visera.Core.Media.Image;
import Visera.Runtime.Render.RHI;

export namespace VE
{

    class FTexture2D
    {
    public:
        inline auto
        Create(SharedPtr<FImage> _Image) { return CreateSharedPtr<FTexture2D>(_Image); }

        FTexture2D() = delete;
        FTexture2D(SharedPtr<FImage> _Image);
        ~FTexture2D();

    private:
        WeakPtr<FImage> ImageReference;
        SharedPtr<RHI::FImage> RHIImage;
        SharedPtr<RHI::FImageView> RHIImageView;
    };

    FTexture2D::
    FTexture2D(SharedPtr<FImage> _Image)
        :ImageReference{_Image}
    {
        //Check hardware supports and convert to valid format.
        if (!_Image->IsBitmap())
        {
            VE_LOG_ERROR("Currently NOT support HDR/PFM/TIFF/GrayScale...!");
            return;
        }

        // Note: FreeImage's Bitmaps are BGRA order by default, so we handle this order first.
        RHI::FSwizzle ImageSwizzle{};
        RHI::EFormat RHIImageFormat{RHI::EFormat::None};
        switch (_Image->GetColorType())
        {
        case FImage::EColorType::RGB:
        {
            if(RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_Normalized_B8_G8_R8))
            {
                RHIImageFormat = RHI::EFormat::U24_Normalized_B8_G8_R8;
                break;
            }
            if(RHI::IsTexture2DFormatSupported(RHI::EFormat::U24_Normalized_R8_G8_B8))
            {
                RHIImageFormat = RHI::EFormat::U24_Normalized_R8_G8_B8;
                ImageSwizzle.R = RHI::ESwizzle::B;
                ImageSwizzle.B = RHI::ESwizzle::R;
                break;
            }
            if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_Normalized_B8_G8_R8_A8))
            {
                VE_LOG_WARN("Current platform do NOT support RGB image format, the image was converted to RGBA!");
                _Image->ConvertToSRGB();
                RHIImageFormat = RHI::EFormat::U32_Normalized_B8_G8_R8_A8;
                break;
            }
            if (RHI::IsTexture2DFormatSupported(RHI::EFormat::U32_Normalized_R8_G8_B8_A8))
            {
                VE_LOG_WARN("Current platform do NOT support RGB image format, the image was converted to RGBA!");
                _Image->ConvertToSRGB();
                RHIImageFormat = RHI::EFormat::U32_Normalized_R8_G8_B8_A8;
                ImageSwizzle.R = RHI::ESwizzle::B;
                ImageSwizzle.B = RHI::ESwizzle::R;
                break;
            }
            VE_LOG_ERROR("Failed to find a suitable format for the image ({})!", _Image->GetPath().ToPlatformString());
            break;
        }
        case FImage::EColorType::RGBA:
        {

            break;
        }
        default:
            VE_LOG_ERROR("Unsupported Texture2D format ({}) of the image ({})!", UInt32(_Image->GetFormat()), _Image->GetPath().ToPlatformString());
        }

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
    }

    FTexture2D::
    ~FTexture2D()
    {

    }

} // namespace VE