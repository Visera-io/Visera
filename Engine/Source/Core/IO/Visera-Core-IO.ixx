module;
#include <Visera.h>

export module Visera.Core.IO;
import :File;
import :BinaryFile;

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