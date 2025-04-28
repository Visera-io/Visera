module;
#include <Visera.h>
#include <FreeImagePlus.h>
#include <png.h>
export module Visera.Core.Media.Image;
#define VE_MODULE_NAME "Image"
export import :Interface;
export import :PNG;
export import :EXR;
import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.Math.Basic;
import Visera.Core.OS.Memory;

export namespace VE
{

	class FImage //[TODO] Remove
	{
	public:
		enum class EColorType
		{
			White	= FIC_MINISWHITE,		//! min value is white
			Black	= FIC_MINISBLACK,		//! min value is black
			RGB		= FIC_RGB       ,		//! RGB color model
			Palette = FIC_PALETTE	,		//! color map indexed
			RGBA	= FIC_RGBALPHA  ,		//! RGB color model with alpha channel
			CMYK	= FIC_CMYK      		//! CMYK color model
		};

		enum class EFormat
		{
			Unknown			= FIT_UNKNOWN,	//! unknown type
			Bitmap			= FIT_BITMAP,	//! standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit
			RGB				= FIT_RGBF,		//! 96-bit RGB float image	: 3 x 32-bit IEEE floating point
			RGBA			= FIT_RGBAF,	//! 128-bit RGBA float image: 4 x 32-bit IEEE floating point
			RGBLite			= FIT_RGB16,	//! 48-bit RGB image		: 3 x 16-bit
			RGBALite		= FIT_RGBA16,	//! 64-bit RGBA image		: 4 x 16-bit

			ArrayUInt16		= FIT_UINT16,	//! array of unsigned short	: unsigned 16-bit
			ArrayInt16		= FIT_INT16,	//! array of short			: signed 16-bit
			ArrayUInt32		= FIT_UINT32,	//! array of unsigned long	: unsigned 32-bit
			ArrayInt32		= FIT_INT32,	//! array of long			: signed 32-bit
			ArrayFloat		= FIT_FLOAT,	//! array of float			: 32-bit IEEE floating point
			ArrayDouble		= FIT_DOUBLE,	//! array of double			: 64-bit IEEE floating point
			ArrayComplex	= FIT_COMPLEX,	//! array of FICOMPLEX		: 2 x 64-bit IEEE floating point
		};

		enum class EType
		{
			PNG  = FIF_PNG,
			JPEG = FIF_JPEG,
		};

		enum class EFilter
		{
			Box = FREE_IMAGE_FILTER::FILTER_BOX,
			Bicubic = FREE_IMAGE_FILTER::FILTER_BICUBIC,
			BSpline = FREE_IMAGE_FILTER::FILTER_BSPLINE,
			Lanczos3 = FREE_IMAGE_FILTER::FILTER_LANCZOS3,
			Bilinear = FREE_IMAGE_FILTER::FILTER_BILINEAR,
			CatmullRom = FREE_IMAGE_FILTER::FILTER_CATMULLROM,
		};

		struct FPixel
		{
			//[TODO]: Pixel Iterator (Avoid the SLOW Boundary Check)
			union
			{
				struct { UInt8 B, G, R, A; };
				RGBQUAD Data{ 0, 0, 0, 255 };
			};
		};

		void Resize(UInt32 _NewWidth, UInt32 _NewHeight, EFilter _Filter) { Handle.rescale(_NewWidth, _NewHeight, FREE_IMAGE_FILTER(_Filter)); }
		void Clear(Int32 _ClearValue = 0) { Memory::Memset(Handle.accessPixels(), _ClearValue, (Handle.getBitsPerPixel() >> 3) * GetPixelCount()); }

		void FlipVertical()		{ Handle.flipVertical(); }
		void FlipHorizontal()	{ Handle.flipHorizontal(); }

		auto GetPixel(UInt32 _X, UInt32 _Y) const -> Optional<FPixel> { FPixel Pixel; if (!Handle.getPixelColor(_X, _Y, &Pixel.Data)) { return {}; } else { return Pixel; } }
		Bool SetPixel(UInt32 _X, UInt32 _Y, FPixel _Value) { VE_ASSERT(!IsGrayScale()); return Handle.setPixelColor(_X, _Y, &_Value.Data); }
		auto GetData()		const -> Byte*		{ return Handle.accessPixels(); }
		auto GetHeight()    const -> UInt32		{ return Handle.getHeight(); }
		auto GetWidth()     const -> UInt32		{ return Handle.getWidth(); }
		auto GetBitsPerPixel() const -> UInt32  { return Handle.getBitsPerPixel(); }
		auto GetSize()		   const -> UInt64	{ return (GetPixelCount() * GetBitsPerPixel()) >> 3; } // div8 (Byte)
		auto GetPixelCount()   const -> UInt64	{ return GetHeight() * GetWidth(); }
		auto GetPaletteSize()  const ->UInt32   { return Handle.getPaletteSize(); }
		auto GetFormat()	   const -> EFormat	{ return static_cast<EFormat>(Handle.getImageType()); }
		auto GetType()         const -> EType   { return static_cast<EType>(Handle.getFIF()); }
		auto GetColorType() const -> EColorType { return static_cast<EColorType>(Handle.getColorType()); }
		auto GetPath()      const -> const FPath& { return Path; }

		Bool IsBitmap()     const { return GetFormat() == EFormat::Bitmap; }
		Bool IsValid()		const { return Handle.isValid(); }
		Bool IsSRGB()       const ;
		Bool IsGrayScale()	const { return Handle.isGrayscale(); }
		Bool HasAlpha()		const { return FreeImage_GetBPP(Handle) == 32; }

		void AdjustGamma(Double _Gamma) { ConvertToLinear(); Handle.adjustGamma(_Gamma); Gamma = _Gamma; } //[(Darken), 1.0, (Lighten)]
		void ConvertToLinear() { if (Gamma > 0.0) { Handle.adjustGamma(1.0/Gamma); } Gamma = 0.0; }
		void ConvertToRGBA32() { if (!Handle.convertTo32Bits()) { VE_LOG_WARN("Failed to convert image ({}) to SRGB!", Path.ToPlatformString()); } }

		void SaveAs(const FPath& _Path) const;
		void Save() const { SaveAs(Path); }

		FImage() = delete;
		FImage(UInt32 _Width, UInt32 _Height, UInt32 _BPP = 4*8);
		FImage(const FPath& _Path);
		~FImage() = default;

	private:
		void LoadFreeImage() const { const static FFreeImage FreeImage; }

	private:
		const	FPath		Path;
		mutable fipImage	Handle;

		mutable Optional<Bool> bSRGB;
		mutable Double         Gamma = 0.0;

		//FreeImagePlus Doc: https://freeimage.sourceforge.io/fip/index.html
		struct FFreeImage
		{
			FFreeImage()  { FreeImage_Initialise();   };
			~FFreeImage() { FreeImage_DeInitialise(); };
		};
	};

	FImage::
	FImage(const FPath& _Path) : Path{_Path}
	{
		LoadFreeImage();

		const String PlatformPath = Path.ToPlatformString();

		if (!Handle.load(PlatformPath.data()))
		{
			String ErrorInfo = Text("Failed to load the image({})! -- throw (SIOFailure).", PlatformPath);
			VE_LOG_ERROR("{}", ErrorInfo);
			throw SIOFailure(ErrorInfo);
		}
		VE_LOG_DEBUG("Loaded a new image from {} (extent:[W:{},H:{},Bits:{}], sRGB:{}).",
			PlatformPath,GetWidth(), GetHeight(), GetBitsPerPixel(), IsSRGB());
	}

	FImage::
	FImage(UInt32 _Width, UInt32 _Height, UInt32 _BPP/* = 4*8 */)
	{
		LoadFreeImage();

		VE_ASSERT(_Width > 0 && _Height > 0 && _BPP > 0 && _BPP % 8 == 0);

		if (!Handle.setSize(FIT_BITMAP, _Width, _Height, _BPP))
		{
			String ErrorInfo = Text("Failed to create a blank image ({}x{}x{})! -- throw(SRuntimeError)", _Width, _Height, _BPP / 4);
			VE_LOG_WARN("{}", ErrorInfo);
			throw SRuntimeError(ErrorInfo);
		}

		bSRGB = False; //Disable SRGB Check
		VE_LOG_DEBUG("Created a new empty image (extent:[W:{},H:{},Bits:{}], sRGB:{}).",
			GetWidth(), GetHeight(), GetBitsPerPixel(), IsSRGB());
	}

	Bool FImage::
	IsSRGB() const
	{
		if (bSRGB.has_value()) { return bSRGB.value(); }

		switch(GetType())
		{
			case EType::PNG:
			{
				for (int i = 0; i <= 11; ++i)
				{
					UInt32 a = Handle.getMetadataCount(FREE_IMAGE_MDMODEL(i));
					if (a) VE_LOG_INFO("{}, {}", i, a);
				}
				
				UInt32 b = Handle.getMetadataCount(FIMD_XMP);
				FITAG* TagA;
				for (int i = 0; i < b; ++i)
				{
					FreeImage_FindFirstMetadata(FIMD_XMP, Handle, &TagA);
					VE_LOG_INFO("{}", (const char*)(FreeImage_GetTagKey(TagA)));
				}
				fipTag Tag;
				if (Handle.getMetadata(FIMD_ANIMATION, "sRGB",  Tag) ||
					Handle.getMetadata(FIMD_XMP,       "CreatorTool",   Tag) ||
					Handle.getMetadata(FIMD_XMP,       "photoshop:ColorMode",   Tag)
					)
				{
					bSRGB = True;
					VE_LOG_INFO("{}", static_cast<const char*>(Tag.getValue()));
					//Tag.getValue(); //[TODO] Indent
				}
				if (Handle.getMetadata(FIMD_ANIMATION, "gAMA", Tag))
				{
					bSRGB = True;
					if (const void* Value = Tag.getValue())
					{
						Gamma = 1.0/(*static_cast<const Double*>(Value)); //Usually, the Gamma saved in PNG is 1.0/2.2.
						
						if (!Equal(Gamma, 2.2))
						{ VE_LOG_WARN("The Gamma of image ({}) is {}!", Path.ToPlatformString(), Gamma); }
					}
				}
				else
				{
					if (bSRGB.has_value() && bSRGB.value())
					{
						VE_LOG_WARN("The image({}) is sRGB but the Gamma unknown! (set as 2.2 by default)",
							Path.ToPlatformString());
						Gamma = 2.2;
					}
				}
				break;
			}
			case EType::JPEG:
			{
				VE_WIP;
				break;
			}
			default:
			{
				String ErrorInfo = Text("Failed to judge if the image ({}) is sRGB! -- throw(SRuntimeError)", Path.ToPlatformString());
				VE_LOG_ERROR("{}", ErrorInfo);
				throw SRuntimeError("");
			}
		}

		bSRGB = bSRGB.has_value()? bSRGB.value() : False;
		return bSRGB.value();
	}

	void FImage::
	SaveAs(const FPath& _Path) const
	{ 
		String PlatformPath = _Path.ToPlatformString();
		if (!Handle.save(PlatformPath.data()))
		{
			String ErrorInfo = Text("Failed to save image as {}! -- throw(SIOFailure)", PlatformPath);
			VE_LOG_WARN("{}", ErrorInfo);
			throw SIOFailure(ErrorInfo);
		}
	}

} // namespace VE