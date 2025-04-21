module;
#include <Visera.h>
export module Visera.Core.OS.FileSystem;
#define VE_MODULE_NAME "FileSystem"
export import Visera.Core.OS.FileSystem.File;

import Visera.Core.Log;
export import Visera.Core.Type;

export namespace VE
{

	class FileSystem
	{
		VE_MODULE_MANAGER_CLASS(FileSystem);
	public:
		VE_API IsExistedFile(const FPath& _Path)			-> Bool						{ return std::filesystem::exists(_Path.ToPlatformString()); }
		VE_API CreateFileIfNotExists(const FPath& _Path)	-> void;
		VE_API CreateFile(const FPath& _FilePath)			-> SharedPtr<FFile>			{ return CreateSharedPtr<FFile>(_FilePath); };
		VE_API CreateBinaryFile(const FPath& _FilePath)		-> SharedPtr<FBinaryFile>	{ return CreateSharedPtr<FBinaryFile>(_FilePath); };
		VE_API CreateJSONFile(const FPath& _FilePath)		-> SharedPtr<FJSONFile>     { return CreateSharedPtr<FJSONFile>(_FilePath); }
	};

	void FileSystem::
	CreateFileIfNotExists(const FPath& _Path)
	{
		if (!IsExistedFile(_Path))
 	    {
 		    std::ofstream NewFile(_Path.ToPlatformString().data());
 		    if (NewFile.is_open())
 		    { return NewFile.close(); }

			VE_LOG_ERROR("Failed to create {}", _Path.ToPlatformString());
 	    }
	}
} // namespace VE