module;
#include <Visera.h>
export module Visera.Core.Type:Path;

import :Text;

export namespace VE
{

	class FPath
	{
	public:
#if (VE_IS_WINDOWS_SYSTEM)
		auto ToPlatformString() const -> String	{ return FText::ToUTF8(Data.c_str()); }
#else
		auto ToPlatformString() const -> String	{ return Data.c_str(); }
#endif

		Bool HasFileName() const  { return Data.has_filename(); }
		auto GetFileName() const -> FPath { if (HasFileName()) { return FPath{ Data.filename().c_str() }; } else { return FPath{}; } }
		Bool SetFileName(StringView _FileName) { if (HasFileName()) { Data.replace_filename(_FileName); return True; } else { return False; } }
		Bool RemoveFileName() { if (HasFileName()) { Data.remove_filename(); return True; } return False; }
		
		Bool HasExtension() const { return Data.has_extension();}
		auto GetExtension() const ->FPath { if (HasExtension()) { return { Data.extension().c_str()} ; } else { return FPath{}; } }
		Bool IsEmpty()      const { return Data.empty(); }

		FPath& Join(StringView _Path) { Data.append(_Path); return *this; }
		FPath& Join(const FPath& _Path)   { Data.append(_Path.Data.c_str()); return *this; }
		FPath& operator+=(StringView _Path) { Join(_Path); return *this; }
		FPath& operator+=(const FPath& _Path)   { Join(_Path); return *this; }
		
		FPath() = default;
		FPath(StringView _Path)		:Data{ _Path } { Data.make_preferred(); }
		FPath(WideStringView _Path)	:Data{ _Path } { Data.make_preferred(); }
		FPath(const char* _Path)	:Data{ _Path } { Data.make_preferred(); }
		FPath(const wchar_t* _Path)	:Data{ _Path } { Data.make_preferred(); }
		FPath(const FPath& _Path)	:Data{ _Path.Data } {}
		FPath(FPath&& _Path)		:Data{ std::move(_Path.Data) } {}
		FPath(std::filesystem::path _Path) :Data{ std::move(_Path) } { Data.make_preferred(); }
		void operator=(StringView _Path)		{ Data = _Path; }
		void operator=(WideStringView _Path)	{ Data = _Path; }
		void operator=(const FPath& _Path)		{ Data = _Path.Data; }
		void operator=(FPath&& _Path)			{ Data = std::move(_Path.Data); }

	private:
		std::filesystem::path Data;
	};

} // namespace VE