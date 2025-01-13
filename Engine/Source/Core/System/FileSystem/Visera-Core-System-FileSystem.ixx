module;
#include <Visera.h>
export module Visera.Core.System.FileSystem;
export import :File;
export import :BinaryFile;

import Visera.Core.Signal;

export namespace VE
{

	class FileSystem
	{
	public:
		struct Path
		{
			public: struct APP
			{
				static inline const char* Assets = VISERA_APP_CACHE_DIR;
				static inline const char* Cache  = VISERA_APP_ASSETS_DIR;
			};
			public: struct Engine
			{
				
			};
		};
		static inline Bool
		IsExistedFile(StringView _Path) { return std::filesystem::exists(_Path); }
		static inline void
		CreateFileIfNotExists(const String& _Path);
		static inline auto
		CreateFile(const String& _FilePath) -> SharedPtr<FFile> { return CreateSharedPtr<FFile>(_FilePath); };
		static inline auto
		CreateBinaryFile(const String& _FilePath) -> SharedPtr<FBinaryFile> { return CreateSharedPtr<FBinaryFile>(_FilePath); };
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
}