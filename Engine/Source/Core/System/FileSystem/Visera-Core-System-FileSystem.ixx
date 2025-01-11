module;
#include <Visera.h>
export module Visera.Core.System.FileSystem;
export import :File;
export import :BinaryFile;

export namespace VE
{
	class FFileSystem
	{
	public:
		auto CreateFile(const String& _FilePath) const -> SharedPtr<FFile> { return CreateSharedPtr<FFile>(_FilePath); };
		auto CreateBinaryFile(const String& _FilePath) const -> SharedPtr<FBinaryFile> { return CreateSharedPtr<FBinaryFile>(_FilePath); };
	
	private:
		//[TODO]: Preset Pathes.
	};
}