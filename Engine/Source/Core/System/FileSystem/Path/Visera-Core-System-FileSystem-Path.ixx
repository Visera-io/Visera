module;
#include <Visera.h>
export module Visera.Core.System.FileSystem.Path;

export namespace VE
{
	
	enum class EPath
	{

	};

	using PathString		= std::wstring;
	using PathStringView	= std::wstring_view;

	class FPath
	{
	public:
		auto GetData() const { return Data; }

		Bool HasFileName() const  { return Data.has_filename(); }
		//auto GetFileName() const -> PathString { if (HasFileName()) { return Handle.filename().c_str(); } else { return L""; } }
		Bool SetFileName(StringView _FileName) { if (HasFileName()) { Data.replace_filename(_FileName); return True; } else { return False; } }
		Bool RemoveFileName() { if (HasFileName()) { Data.remove_filename(); return True; } return False; }
		
		Bool HasExtension() const { return Data.has_extension();}
		//auto GetExtension() const ->StringView { if (HasExtension()) { return Handle.extension().c_str(); } else { return ""; } }

		FPath& Join(StringView _Path) { Data.append(_Path); return *this; }
		FPath& Join(const FPath& _Path)   { Data.append(_Path.Data.c_str()); return *this; }
		FPath& operator+=(StringView _Path) { Join(_Path); return *this; }
		FPath& operator+=(const FPath& _Path)   { Join(_Path); return *this; }
		
		FPath() = default;
		FPath(PathString _Path)		:Data{ _Path } { Data.make_preferred(); }
		FPath(PathStringView _Path)	:Data{ _Path } { Data.make_preferred(); }
		FPath(const FPath& _Path)	:Data{ _Path.Data } {}
		FPath(FPath&& _Path)		:Data{ std::move(_Path.Data) } {}
		FPath(std::filesystem::path _Path) :Data{ std::move(_Path) } { Data.make_preferred(); }
		void operator=(PathString _Path)		{ Data = _Path; }
		void operator=(PathStringView _Path)	{ Data = _Path; }
		void operator=(const FPath& _Path)		{ Data = _Path.Data; }
		void operator=(FPath&& _Path)			{ Data = std::move(_Path.Data); }

	private:
		std::filesystem::path Data;
	};

} // namespace VE