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
		VE_API IsExistedFile(const FPath& _Path)			-> Bool						{ return std::filesystem::exists(_Path.GetHandle() ); }
		VE_API CreateFileIfNotExists(const FPath& _Path)	-> void;
		VE_API OpenFile(const FPath& _FilePath)				-> SharedPtr<FFile>			{ return CreateSharedPtr<FFile>(_FilePath); };
		VE_API OpenBinaryFile(const FPath& _FilePath)		-> SharedPtr<FBinaryFile>	{ return CreateSharedPtr<FBinaryFile>(_FilePath); };
	};

	void FileSystem::
	CreateFileIfNotExists(const FPath& _Path)
	{
		if (!IsExistedFile(_Path))
 	    {
 		    std::ofstream NewFile(_Path.GetHandle());
 		    if (NewFile.is_open()) NewFile.close();
			else throw SIOFailure(Text("Failed to create a new file at {}", "_Path"));
 	    }
	}
} // namespace VE