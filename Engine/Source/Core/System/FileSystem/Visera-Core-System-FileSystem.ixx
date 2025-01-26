module;
#include <Visera.h>
export module Visera.Core.System.FileSystem;
export import Visera.Core.System.FileSystem.Path;
export import :File;
export import :BinaryFile;

import Visera.Core.Signal;

export namespace VE
{

	class FileSystem
	{
		VE_MODULE_MANAGER_CLASS(FileSystem);
	public:
		/*struct EPath
		{
			public: struct APP
			{
				static inline const char* Assets = VISERA_APP_CACHE_DIR;
				static inline const char* Cache  = VISERA_APP_ASSETS_DIR;
			};
			public: struct Engine
			{
				
			};
		};*/
		VE_API IsExistedFile(StringView _Path)				-> Bool						{ return std::filesystem::exists(_Path); }
		VE_API CreateFileIfNotExists(const String& _Path)	-> void;
		VE_API CreateFile(const String& _FilePath)			-> SharedPtr<FFile>			{ return CreateSharedPtr<FFile>(_FilePath); };
		VE_API CreateBinaryFile(const String& _FilePath)	-> SharedPtr<FBinaryFile>	{ return CreateSharedPtr<FBinaryFile>(_FilePath); };
	};

	void FileSystem::
	CreateFileIfNotExists(const String& _Path)
	{
		if (!IsExistedFile(_Path))
 	    {
 		    std::ofstream NewFile(_Path.data());
 		    if (NewFile.is_open()) NewFile.close();
 		    else throw SIOFailure(Text("Failed to create a new file at {}", _Path));
 	    }
	}
} // namespace VE