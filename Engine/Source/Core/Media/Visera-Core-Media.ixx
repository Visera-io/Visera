module;
#include <Visera.h>
export module Visera.Core.Media;
import Visera.Core.Media.Image;

export namespace VE
{
	class ViseraCore;

	class Media
	{
		VE_MODULE_MANAGER_CLASS(Media);
		friend class ViseraCore;
	public:
		VE_API CreateImage() -> SharedPtr<FImage> { return CreateSharedPtr<FImage>(); }

	private:
		VE_API Bootstrap() -> void { FreeImage::Bootstrap(); }
		VE_API Terminate() -> void { FreeImage::Terminate(); }
	};

} // namespace VE