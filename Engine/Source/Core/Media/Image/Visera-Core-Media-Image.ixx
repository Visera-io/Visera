module;
#include <Visera.h>
#include <FreeImagePlus.h>
export module Visera.Core.Media.Image;

import Visera.Core.Type;
import Visera.Core.Type;
import Visera.Core.Signal;

export namespace VE
{

	class FImage
	{
	public:
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
		void FlipVertical()		{ Handle.flipVertical(); }
		void FlipHorizontal()	{ Handle.flipHorizontal(); }
		void ToRGBA()			{ Handle.convertToRGBAF(); }

		auto GetPixel(UInt32 _X, UInt32 _Y) const -> Optional<FPixel> { FPixel Pixel; if (!Handle.getPixelColor(_X, _Y, &Pixel.Data)) { return {}; } else { return Pixel; } }
		Bool SetPixel(UInt32 _X, UInt32 _Y, FPixel _Value) { VE_ASSERT(!IsGrayScale()); return Handle.setPixelColor(_X, _Y, &_Value.Data); }
		auto GetData()		const -> Byte*		{ return Handle.accessPixels(); }
		auto GetSize()		const -> UInt64		{ return Handle.getImageSize(); }
		auto GetHeight()    const -> UInt32		{ return Handle.getHeight(); }
		auto GetWidth()     const -> UInt32		{ return Handle.getWidth(); }
		auto GetFormat()	const -> EFormat	{ return static_cast<EFormat>(Handle.getImageType()); }

		Bool IsValid()		const { return Handle.isValid(); }
		Bool IsGrayScale()	const { return Handle.isGrayscale(); }
		Bool HasAlpha()		const { return FreeImage_GetBPP(Handle) == 32; }

		void Save() const { if (!Handle.save(Path.ToPlatformString().data())) { throw SIOFailure(Text("Failed to save the image({})!", Path.ToPlatformString())); } }
		void SaveAs(const FPath& _Path) const { if (!Handle.save(_Path.ToPlatformString().data())) { throw SIOFailure(Text("Failed to save the image({})!", _Path.ToPlatformString())); } }

		FImage() = delete;
		FImage(const FPath& _Path);
		~FImage() = default;

	private:
		const	FPath		Path;
		mutable fipImage	Handle;

		//FreeImagePlus Doc: https://freeimage.sourceforge.io/fip/index.html
		struct FFreeImage
		{
			FFreeImage()  { FreeImage_Initialise();   };
			~FFreeImage() { FreeImage_DeInitialise(); };
		};
		;
	};

	FImage::
	FImage(const FPath& _Path) : Path{_Path}
	{
		const static FFreeImage FreeImage;

		if (!Handle.load(Path.ToPlatformString().data()))
		{ throw SIOFailure(Text("Failed to load the image({})!", Path.ToPlatformString())); }
		
		// Force Convert to 32Bit Bitmap for Vulkan.
		if (!HasAlpha())
		{
			if (!Handle.convertTo32Bits())
			{ throw SRuntimeError(Text("Failed to convert the image({}) to 32Bits!", Path.ToPlatformString())); }
		}
	}

} // namespace VE