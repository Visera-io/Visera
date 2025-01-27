module;
#include <Visera.h>
export module Visera.Core.Media;
import Visera.Core.Media.Image;
import Visera.Core.Type;
import Visera.Core.System.FileSystem;

export namespace VE
{

	class Media
	{
		VE_MODULE_MANAGER_CLASS(Media);
	
		VE_API CreateImage(FName _Name, const FPath& _Path) -> SharedPtr<FImage> { return CreateSharedPtr<FImage>(_Name, _Path); }

	};

} // namespace VE