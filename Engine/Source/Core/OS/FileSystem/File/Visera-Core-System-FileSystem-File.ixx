module;
#include <Visera.h>
export module Visera.Core.OS.FileSystem.File;
export import :Interface;
export import :BinaryFile;

export namespace VE
{

	class FFile : public IFile
	{
	public:
		FFile(const FPath& FilePath) noexcept : IFile{ FilePath } {};
 	    virtual ~FFile() noexcept = default;
	};

} // namespace VE