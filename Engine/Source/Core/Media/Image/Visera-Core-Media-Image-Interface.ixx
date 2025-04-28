module;
#include <Visera.h>
#include <FreeImagePlus.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include <stb_image_resize2.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
export module Visera.Core.Media.Image:Interface;
#define VE_MODULE_NAME "Image:Interface"
import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Math.Basic;
import Visera.Core.OS.Memory;

export namespace VE
{

    enum class EImageType
    {
        Bitmap,
        PNG,
        JPEG,
        EXR,

        Unknown,
    };
    auto inline Text(EImageType _Type) {  switch (_Type) { case EImageType::Bitmap: return "Bitmap"; case EImageType::PNG: return "PNG"; case EImageType::JPEG: return "JPEG"; default: return "Unknown"; } }

    enum class EColorSpace
    {
        RGB,
        sRGB,
        CMYK,

        Unknown,
    };
    auto inline Text(EColorSpace _ColorSpace) {  switch (_ColorSpace) { case EColorSpace::RGB: return "RGB"; case EColorSpace::sRGB: return "sRGB"; case EColorSpace::CMYK: return "CMYK"; default: return "Unknown"; } }

    enum class EImageFormat
    {
        RGB_R8_G8_B8       = STBIR_RGB,
        RGB_B8_G8_R8       = STBIR_BGR,
        RGBA_R8_G8_B8_A8   = STBIR_RGBA,
        RGBA_B8_G8_R8_A8   = STBIR_BGRA,
        RGBA_Float         = STBIR_TYPE_FLOAT,
        RGBA_HalfFloat     = STBIR_TYPE_HALF_FLOAT,

        Gray               = STBIR_1CHANNEL,
        GrayAlpha          = STBIR_2CHANNEL,

        Unknown,
    };
    inline auto Text(EImageFormat _Format) { switch (_Format) { case EImageFormat::RGB_R8_G8_B8: return "RGB_R8_G8_B8"; case EImageFormat::RGB_B8_G8_R8: return "RGB_B8_G8_R8"; case EImageFormat::RGBA_R8_G8_B8_A8: return "RGBA_R8_G8_B8_A8"; case EImageFormat::RGBA_B8_G8_R8_A8: return "RGBA_B8_G8_R8_A8"; case EImageFormat::Gray: return "Gray"; case EImageFormat::GrayAlpha: return "GrayAlpha"; default: return "Unknown"; } }


    class IImage : public std::enable_shared_from_this<IImage>
    {
        VE_NOT_COPYABLE(IImage);
    public:
        void inline
        Resize(UInt32 _NewWidth, UInt32 _NewHeight);
        void inline
        ConvertToRGBA(UInt8 _Alpha = 255);
        void inline
        FlipVertically();

        auto inline
        GetData()       const -> const void*  { return Data.data(); }
        auto inline
        GetPath()       const -> const FPath& { return Path; }
        auto inline
        GetMemorySize() const { return Data.size(); }
        auto inline
        GetWidth()      const { return Width;    }
        auto inline
        GetHeight()     const { return Height;   }
        auto inline
        GetChannels()   const { return Channels; }
        auto inline
        GetColorSpace() const { return ColorSpace; }
        auto inline
        GetFormat()     const { return Format; }
        auto inline
        GetGamma()      const { return Gamma; }
        auto inline
        GetSRGBIntent() const { return SRGBIntent; }
        auto inline
        GetInfo()       const -> String;

        Bool inline
        IsEmpty()                const { return Data.empty(); }
        Bool inline
        IsSRGB()                 const { return ColorSpace == EColorSpace::sRGB; }
        Bool inline
        IsGrayScale()            const { return Format == EImageFormat::Gray || Format == EImageFormat::GrayAlpha; }
        Bool inline
        IsType(EImageType _Type) const { return Type == _Type; }
        Bool inline
        IsLoadedFromFile()       const { return !Path.IsEmpty(); }

        IImage() = delete;
        IImage(EImageType _Type, const FPath& _Path) : Type{_Type}, Path{_Path} {}
        virtual ~IImage() = default;

    protected:
        Array<Byte>  Data;
        UInt32       Width{0}, Height{0}, Channels{0};
        EImageType   Type       = EImageType::Unknown;
        EImageFormat Format     = EImageFormat::Unknown;
        EColorSpace  ColorSpace = EColorSpace::Unknown;

        Int32        SRGBIntent{-1};
        Double       Gamma{1.0};

        FPath        Path;
    };

    void IImage::
    FlipVertically()
    {
        const UInt64 RowBytes = Width * Channels;
        Byte* SwapBuffer = static_cast<Byte*>(Memory::Malloc(RowBytes, 8));
        if(!SwapBuffer)
        {
            VE_LOG_ERROR("Failed to allocate memory! (func:{}, line:{})",
                        __FUNCTION__, __LINE__);
            return;
        }
        for (UInt32 Row = 0; Row < Height / 2; Row++)
        {
            Byte* Top    = &Data[Row * RowBytes];
            Byte* Bottom = &Data[(Height - Row - 1) * RowBytes];
            Memory::Memcpy(SwapBuffer, Top, RowBytes);
            Memory::Memcpy(Top, Bottom, RowBytes);
            Memory::Memcpy(Bottom, SwapBuffer, RowBytes);
        }
        Memory::Free(SwapBuffer, 8);
    }

    String IImage::
    GetInfo() const
    {
        return Text("(file:\"{}\", extent:[W{},H{},C{}], type:{}, format:{}, color space:{})",
            Path.GetFileName().ToPlatformString(), GetWidth(), GetHeight(), GetChannels(),
            Text(Type), Text(Format), Text(ColorSpace));
    }

    void IImage::
    Resize(UInt32 _NewWidth, UInt32 _NewHeight)
    {
        VE_ASSERT(_NewWidth && _NewHeight);
        VE_ASSERT(Format != EImageFormat::Unknown);

        Array<Byte> NewData(_NewWidth * _NewHeight * GetChannels());
        if (IsSRGB())
        {
            stbir_resize_uint8_srgb(
                Data.data(), Width, Height, 0,
                NewData.data(), _NewWidth, _NewHeight, 0,
                static_cast<stbir_pixel_layout>(Format));
        }
        else
        {
            stbir_resize_uint8_linear(
                Data.data(), Width, Height, 0,
                NewData.data(), _NewWidth, _NewHeight, 0,
                static_cast<stbir_pixel_layout>(Format));
        }

        Data   = std::move(NewData);
        Width  = _NewWidth;
        Height = _NewHeight;
        VE_LOG_DEBUG("The image ({}) was resized from [W{},H{}] to [W{},H{}].",
            Path.GetFileName().ToPlatformString(),
            Width, Height, _NewWidth, _NewHeight);
    }

    void IImage::
    ConvertToRGBA(UInt8 _Alpha /*= 255*/)
    {
        if (Channels == 4) { return; }

        const UInt64 Resolution = Width * Height;
        _Alpha = GetClamped(_Alpha, 0, 255);
        Array<Byte> NewData(Resolution * 4);

        if (IsGrayScale())
        {
            for (UInt64 Idx = 0; Idx < Resolution; ++Idx)
            {
                NewData[Idx * 4 + 0] = Data[Idx]; // R = R
                NewData[Idx * 4 + 1] = Data[Idx]; // G = R
                NewData[Idx * 4 + 2] = Data[Idx]; // B = R
                NewData[Idx * 4 + 3] = _Alpha;    // A
            }
        }
        else
        {
            for (UInt64 Idx = 0; Idx < Resolution; ++Idx)
            {
                NewData[Idx * 4 + 0] = Data[Idx * 3 + 0]; // R = R
                NewData[Idx * 4 + 1] = Data[Idx * 3 + 1]; // G = G
                NewData[Idx * 4 + 2] = Data[Idx * 3 + 2]; // B = B
                NewData[Idx * 4 + 3] = _Alpha;            // A
            }
        }

        Format   = EImageFormat::RGBA_R8_G8_B8_A8;
        Data     = std::move(NewData);
        Channels = 4;
        VE_LOG_DEBUG("The image {} was converted to RGBA.", GetInfo());
    }

} // namespace VE