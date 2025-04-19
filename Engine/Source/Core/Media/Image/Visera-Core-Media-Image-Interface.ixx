module;
#include <FreeImagePlus.h>
#include <Visera.h>
export module Visera.Core.Media.Image:Interface;
#define VE_MODULE_NAME "Image:Interface"
import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Math.Basic;

export namespace VE
{

    enum class EImageType
    {
        Bitmap,
        PNG,
        JPEG,

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
        RGB_R8_G8_B8,
        RGB_B8_G8_R8,
        RGBA_R8_G8_B8_A8,
        RGBA_B8_G8_R8_A8,
        RGB_Float,
        RGBA_Float,

        Gray,
        GrayAlpha,

        Unknown,
    };
    inline auto Text(EImageFormat _Format) { switch (_Format) { case EImageFormat::RGB_R8_G8_B8: return "RGB_R8_G8_B8"; case EImageFormat::RGB_B8_G8_R8: return "RGB_B8_G8_R8"; case EImageFormat::RGBA_R8_G8_B8_A8: return "RGBA_R8_G8_B8_A8"; case EImageFormat::RGBA_B8_G8_R8_A8: return "RGBA_B8_G8_R8_A8"; case EImageFormat::RGB_Float: return "RGB_Float"; case EImageFormat::RGBA_Float: return "RGBA_Float"; default: return "Unknown"; } }


    class IImage : public std::enable_shared_from_this<IImage>
    {
        VE_NOT_COPYABLE(IImage);
    public:
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

    String IImage::
    GetInfo() const
    {
        return Text("(file:\"{}\", extent:[W{},H{},C{}]), type:{}, format:{}, color space:{})",
            Path.GetFileName().ToPlatformString(), GetWidth(), GetHeight(), GetChannels(),
            Text(Type), Text(Format), Text(ColorSpace));
    }

} // namespace VE