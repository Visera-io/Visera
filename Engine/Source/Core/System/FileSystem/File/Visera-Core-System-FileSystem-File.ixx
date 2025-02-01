module;
#include <Visera.h>
export module Visera.Core.System.FileSystem.File;
import :BasicFile;
export import :BinaryFile;

export namespace VE
{

	class FFile : public FBasicFile
	{
	public:
		FFile(const FPath& FilePath) noexcept : FBasicFile{ FilePath } {};
 	    virtual ~FFile() noexcept = default;
	};

} // namespace VE