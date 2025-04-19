module;
#include <Visera.h>
#include <png.h>
export module Visera.Core.Media.Image:PNG;
#define VE_MODULE_NAME "Image:PNG"
import :Interface;

import Visera.Core.Log;
import Visera.Core.Signal;
import Visera.Core.Type;

export namespace VE
{

    class FPNGImage : public IImage
    {
    public:
        FPNGImage() = delete;
        FPNGImage(const FPath& _Path);

    private:
        void StartParsing(FILE** File_, png_structp* PNGPtr_, png_infop* InfoPtr_);
        void Preprocessing(const png_structp _PNGPtr, const png_infop _InfoPtr);
        void DetectFormat(const png_structp _PNGPtr, const png_infop _InfoPtr);
        void DetectColorSpace(const png_structp _PNGPtr, const png_infop _InfoPtr);
        void LoadData(const png_structp _PNGPtr, const png_infop _InfoPtr);
        void StopParsing(FILE** _File, png_structp* _PNGPtr, png_infop* _InfoPtr);
    };

    FPNGImage::
    FPNGImage(const FPath& _Path)
        : IImage{ EImageType::PNG, _Path }
    {
        VE_ASSERT(!_Path.IsEmpty());
        FILE*       PNGFile = nullptr;
        png_structp PNGPtr  = nullptr;
        png_infop   InfoPtr = nullptr;

        StartParsing(&PNGFile, &PNGPtr, &InfoPtr);
        {
            Preprocessing(PNGPtr,  InfoPtr);

            Width    = png_get_image_width(PNGPtr,  InfoPtr);
            Height   = png_get_image_height(PNGPtr, InfoPtr);
            Channels = png_get_channels(PNGPtr,     InfoPtr);

            DetectFormat(PNGPtr, InfoPtr);
            DetectColorSpace(PNGPtr, InfoPtr);

            LoadData(PNGPtr, InfoPtr);
        }
        StopParsing(&PNGFile, &PNGPtr, &InfoPtr);

        VE_LOG_INFO("Created a new PNG image {}.", GetInfo());
    }

    void FPNGImage::
    Preprocessing(const png_structp _PNGPtr, const png_infop _InfoPtr)
    {
        const auto ColorType = png_get_color_type(_PNGPtr, _InfoPtr);
        const auto BitDepth  = png_get_bit_depth(_PNGPtr, _InfoPtr);
        if (ColorType == PNG_COLOR_TYPE_PALETTE) { png_set_palette_to_rgb(_PNGPtr); }
        if (ColorType == PNG_COLOR_TYPE_GRAY && BitDepth < 8) { png_set_expand_gray_1_2_4_to_8(_PNGPtr); }
        // Force to be 8bits (enough for human's visual perception)
        if (BitDepth > 8) { if (BitDepth == 16) { png_set_strip_16(_PNGPtr); } else 
                            { VE_LOG_WARN("Unsupported bit depth ({}) of image ({})", BitDepth, Path.ToPlatformString()); } }
        if (BitDepth < 8) { png_set_packing(_PNGPtr); }
        png_read_update_info(_PNGPtr, _InfoPtr); // Once you update the data, you MUST call "png_read_update_info"
    }

    void FPNGImage::
    LoadData(const png_structp _PNGPtr, const png_infop _InfoPtr)
    {
        const UInt64 RowBytes = png_get_rowbytes(_PNGPtr, _InfoPtr);
        Data.resize(RowBytes * Height);
        for (UInt32 Row = 0; Row < Height; ++Row)
        {
            png_read_row(_PNGPtr, &Data[(Row) * RowBytes], nullptr); // Flip Vertically

            //png_read_row(_PNGPtr, &Data[(Height - Row - 1) * RowBytes], nullptr); // Flip Vertically
        }
    }

    void FPNGImage::
    DetectFormat(const png_structp _PNGPtr, const png_infop _InfoPtr)
    {
        //libpng read PNG images in RGBA order.
        const auto ColorType = png_get_color_type(_PNGPtr, _InfoPtr);
        switch (ColorType)
        {
            case PNG_COLOR_TYPE_RGB:
                Format = EImageFormat::RGB_R8_G8_B8;
            break;
            case PNG_COLOR_TYPE_RGB_ALPHA:
                Format = EImageFormat::RGBA_R8_G8_B8_A8;
            break;
            case PNG_COLOR_TYPE_GRAY:
                VE_LOG_WARN("The gray image ({}) is NOT safely supported!", Path.ToPlatformString())
                Format = EImageFormat::Gray;
            break;
            case PNG_COLOR_TYPE_GRAY_ALPHA:
                VE_LOG_WARN("The gray alpha image ({}) is NOT safely supported!", Path.ToPlatformString())
                Format = EImageFormat::GrayAlpha;
            break;
            default:
                VE_LOG_WARN("The format of image ({}) is NOT supported!", Path.ToPlatformString())
                Format = EImageFormat::Unknown;
            break;
        }
    }

    void FPNGImage::
    DetectColorSpace(const png_structp _PNGPtr, const png_infop _InfoPtr)
    {
        ColorSpace = EColorSpace::RGB;
        if (png_get_valid(_PNGPtr, _InfoPtr, PNG_INFO_sRGB))
        {
            png_get_sRGB(_PNGPtr, _InfoPtr, &SRGBIntent);
            ColorSpace = EColorSpace::sRGB;
        }

        if (png_get_valid(_PNGPtr, _InfoPtr, PNG_INFO_gAMA))
        {
            png_get_gAMA(_PNGPtr, _InfoPtr, &Gamma);
            ColorSpace = EColorSpace::sRGB;
        }
        else
        {
            if (IsSRGB())
            {
                VE_LOG_WARN("The image ({}) is sRGB but the gAMA was not found in the metadata, set as 1.0/2.2 by default!", Path.ToPlatformString());
                Gamma = 1.0/2.2;
            }
        }
    }

    void FPNGImage::
    StartParsing(FILE** File_, png_structp* PNGPtr_, png_infop* InfoPtr_)
    {
        VE_ASSERT(File_ && *File_ == nullptr && "Do NOT pass an opened FILE!");

        const String PlatformPath = Path.ToPlatformString();

        FILE* File = fopen(PlatformPath.c_str(), "rb");
        if (!File)
        {
            String ErrorInfo = Text("Failed to read the image ({})! -- throw(SIOFailure)", PlatformPath);
            VE_LOG_ERROR("{}", ErrorInfo);
            throw SIOFailure(ErrorInfo);
        }

        png_byte header[8];
        if (fread(header, 1, 8, File) != 8)
        {
            String ErrorInfo = Text("Failed to read PNG signature bytes from ({}).", PlatformPath);
            VE_LOG_ERROR("{}", ErrorInfo);
            fclose(File);
            throw SIOFailure(ErrorInfo);
        }

        if (png_sig_cmp(header, 0, 8))
        {
            String ErrorInfo = Text("The image ({}) is an invalid PNG! -- throw(SIOFailure)", PlatformPath);
            VE_LOG_ERROR("{}", ErrorInfo);
            fclose(File);
            throw SIOFailure(ErrorInfo);
        }

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr)
        {
            String ErrorInfo = Text("Failed to png_create_read_struct({})! -- throw(SIOFailure)", PlatformPath);
            VE_LOG_ERROR("{}", ErrorInfo);
            fclose(File);
            throw SIOFailure(ErrorInfo);
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
        {
            String ErrorInfo = Text("Failed to png_create_info_struct({})! -- throw(SIOFailure)", PlatformPath);
            VE_LOG_ERROR("{}", ErrorInfo);
            png_destroy_read_struct(&png_ptr, nullptr, nullptr);
            fclose(File);
            throw SIOFailure(ErrorInfo);
        }

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            String ErrorInfo = Text("Error during processing({})! -- throw(SIOFailure)", PlatformPath);
            VE_LOG_ERROR("{}", ErrorInfo);
            png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
            fclose(File);
            throw SIOFailure(ErrorInfo);
        }

        png_init_io(png_ptr, File);
        png_set_sig_bytes(png_ptr, 8); // We already read 8 bytes from the header
        
        png_read_info(png_ptr, info_ptr); //MUST read metadata at first

        *File_    = File;
        *PNGPtr_  = png_ptr;
        *InfoPtr_ = info_ptr;
    }

    void FPNGImage::
    StopParsing(FILE** _File, png_structp* _PNGPtr, png_infop* _InfoPtr)
    {
        VE_ASSERT(_File && *_File);
        fclose(*_File);
        png_destroy_read_struct(_PNGPtr, _InfoPtr, nullptr);
    }

} // namespace VE