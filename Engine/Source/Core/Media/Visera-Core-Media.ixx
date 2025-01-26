module;
#include <Visera.h>
export module Visera.Core.Media;
import Visera.Core.Media.Image;
import Visera.Core.Type;
import Visera.Core.System.FileSystem;

export namespace VE
{
	class ViseraCore;

	class Media
	{
		VE_MODULE_MANAGER_CLASS(Media);
		friend class ViseraCore;
	public:
		VE_API CreateImage(FName _Name, const FPath& _Path) -> SharedPtr<FImage> { return CreateSharedPtr<FImage>(_Name, _Path); }

	private:
		VE_API Bootstrap() -> void { FreeImage::Bootstrap(); }
		VE_API Terminate() -> void { FreeImage::Terminate(); }
	};

} // namespace VE