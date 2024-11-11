module;
#include <Visera>

export module Visera.Engine.Core.IO;
import Visera.Engine.Core.IO.File;

export namespace VE
{	
	#define INTERFACE static inline auto

	class IO
	{
	public:
		using File = VE::File;
		INTERFACE CreateBinaryFile(const String& Path) -> SharedPtr<File> { return CreateSharedPtr<BinaryFile>(Path); }
	};

} // namespace VE