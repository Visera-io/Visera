module;
#include <Visera.h>
#include <FreeImagePlus.h>
export module Visera.Core.Media.Image;
#define VE_MODULE_NAME "Image"
import Visera.Core.Log;
import Visera.Core.Type;
import Visera.Core.Signal;
import Visera.Core.OS.Memory;

export namespace VE
{

	class FImage
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
			Unkonwn			= FIT_UNKNOWN,	//! unknown type
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
		auto GetSize()		const -> UInt64		{ return Handle.getImageSize(); }
		auto GetHeight()    const -> UInt32		{ return Handle.getHeight(); }
		auto GetWidth()     const -> UInt32		{ return Handle.getWidth(); }
		auto GetPixelCount()const -> UInt64		{ return GetHeight() * GetWidth(); }
		auto GetFormat()	const -> EFormat	{ return static_cast<EFormat>(Handle.getImageType()); }
		auto GetColorType() const -> EColorType { return ColorType; }

		Bool IsValid()		const { return Handle.isValid(); }
		Bool IsGrayScale()	const { return Handle.isGrayscale(); }
		Bool IsSRGB()		const { return ColorType == EColorType::RGBA || ColorType == EColorType::RGB; }
		Bool HasAlpha()		const { return FreeImage_GetBPP(Handle) == 32; }

		void ConvertToSRGB()		{ Handle.convertTo32Bits(); }

		void Save() const { if (!Handle.save(Path.ToPlatformString().data())) { throw SIOFailure(Text("Failed to save the image({})!", Path.ToPlatformString())); } }
		void SaveAs(const FPath& _Path) const { if (!Handle.save(_Path.ToPlatformString().data())) { throw SIOFailure(Text("Failed to save the image({})!", _Path.ToPlatformString())); } }

		FImage() = delete;
		FImage(UInt32 _Width, UInt32 _Height, UInt32 _BPP = 4*8);
		FImage(const FPath& _Path);
		~FImage() = default;

	private:
		void LoadFreeImage() const { const static FFreeImage FreeImage; }

	private:
		const	FPath		Path;
		mutable fipImage	Handle;
		EColorType			ColorType;

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
		VE_LOG_DEBUG("Loading a new image from {}", Path.ToPlatformString());

		if (!Handle.load(Path.ToPlatformString().data()))
		{ throw SIOFailure(Text("Failed to load the image({})!", Path.ToPlatformString())); }
		
		// Force Convert to 32Bit Bitmap for Vulkan.
		if (!HasAlpha())
		{
			if (!Handle.convertTo32Bits())
			{ throw SRuntimeError(Text("Failed to convert the image({}) to 32Bits!", Path.ToPlatformString())); }
		}

		ColorType = static_cast<EColorType>(Handle.getColorType());
	}

	FImage::
	FImage(UInt32 _Width, UInt32 _Height, UInt32 _BPP/* = 4*8 */)
	{
		LoadFreeImage();
		VE_ASSERT(_Width > 0 && _Height > 0 && _BPP > 0 && _BPP % 8 == 0);

		if (!Handle.setSize(FIT_BITMAP, _Width, _Height, _BPP))
		{ throw SRuntimeError(Text("Failed to create a blank image ({}x{}x{})!", _Width, _Height, _BPP / 4)); }

		ColorType = static_cast<EColorType>(Handle.getColorType());
	}

} // namespace VE