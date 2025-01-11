module;
#include <Visera.h>
export module Visera.Core.System;
import Visera.Core.System.FileSystem;

import Visera.Core.Time;
import Visera.Core.Signal;

export namespace VE
{
	class System
	{
	public:
		static inline auto
		Now() { return SystemClock.Now(); }
		static inline auto
		GetRunningTime() { return SystemClock.GetTotalTime(); }

		constexpr Bool
		IsLittleEndian() { return std::endian::native == std::endian::little; }

		static inline Bool
		IsExistedFile(StringView _Path) { return std::filesystem::exists(_Path); }
		static inline void
		CreateFileIfNotExists(const String& _Path);
		static inline auto
		CreateFile(const String& Path)		 -> SharedPtr<FFile>		{ return FileSystem.CreateFile(Path); }
		static inline auto
		CreateBinaryFile(const String& Path) -> SharedPtr<FBinaryFile>	{ return FileSystem.CreateBinaryFile(Path); }

	private:
		static inline FSystemClock	SystemClock;
		static inline FFileSystem	FileSystem;
	};

	void System::
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