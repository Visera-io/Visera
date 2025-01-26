module;
#include <Visera.h>
#include <FreeImagePlus.h>
export module Visera.Core.Media.Image;

import Visera.Core.Signal;
import Visera.Core.Type;

export namespace VE
{
	class Media;

	//FreeImagePlus Doc: https://freeimage.sourceforge.io/fip/index.html
	class FreeImage
	{		
		friend class Media;
		static inline void Bootstrap() { FreeImage_Initialise();   };
		static inline void Terminate() { FreeImage_DeInitialise(); };
	};

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

		struct FPixel
		{
			union
			{
				Byte	R, G, B, A;
				RGBQUAD Data{ 255, 0, 255, 255 };
			};
		};
		
		auto GetPixel(UInt32 _X, UInt32 _Y) const -> Optional<FPixel> { FPixel Pixel; if (!Handle.getPixelColor(_X, _Y, &Pixel.Data)) { return {}; } else { return Pixel; } }
		Bool SetPixel(UInt32 _X, UInt32 _Y, FPixel _Value) { VE_ASSERT(!IsGrayScale()); return Handle.setPixelColor(_X, _Y, &_Value.Data); }
		auto GetData()		const -> Byte*		{ return Handle.accessPixels(); }
		auto GetSize()		const -> UInt64		{ return Handle.getImageSize(); }
		auto GetHeight()    const -> UInt32		{ return Handle.getHeight(); }
		auto GetWidth()     const -> UInt32		{ return Handle.getWidth(); }
		auto GetFormat()	const -> EFormat	{ return static_cast<EFormat>(Handle.getImageType()); }

		Bool IsValid()		const { return Handle.isValid(); }
		Bool IsGrayScale()	const { return Handle.isGrayscale(); }

		void Save() const { if (!Handle.save(Text("D:\\Downloads\\{}.png", Name.GetNameWithNumber()).c_str())) { throw SIOFailure(Text("Failed to save the image({}) to {}!", Name.GetNameWithNumber(), Path)); } }

		FImage();
		~FImage() = default;

	private:
		FName  Name;
		String Path = "D:\\Downloads\\Figure 11.8.png";
		mutable fipImage Handle;
	};

	FImage::
	FImage() : Name{"TestImage"}
	{
		if (!Handle.load(Path.c_str()))
		{ throw SIOFailure(Text("Failed to load the image from {}!", Path)); }
	}

} // namespace VE