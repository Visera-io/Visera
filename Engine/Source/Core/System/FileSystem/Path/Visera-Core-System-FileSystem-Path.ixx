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
		auto GetHandle() const { return Handle; }

		Bool HasFileName() const  { return Handle.has_filename(); }
		//auto GetFileName() const -> PathString { if (HasFileName()) { return Handle.filename().c_str(); } else { return L""; } }
		Bool SetFileName(StringView _FileName) { if (HasFileName()) { Handle.replace_filename(_FileName); return True; } else { return False; } }
		Bool RemoveFileName() { if (HasFileName()) { Handle.remove_filename(); return True; } return False; }
		
		Bool HasExtension() const { return Handle.has_extension();}
		//auto GetExtension() const ->StringView { if (HasExtension()) { return Handle.extension().c_str(); } else { return ""; } }

		FPath& Join(StringView _Path) { Handle.append(_Path); return *this; }
		FPath& Join(const FPath& _Path)   { Handle.append(_Path.Handle.c_str()); return *this; }
		FPath& operator+=(StringView _Path) { Join(_Path); return *this; }
		FPath& operator+=(const FPath& _Path)   { Join(_Path); return *this; }
		
		FPath() = default;
		FPath(PathString _Path)		:Handle{ _Path } { Handle.make_preferred(); }
		FPath(PathStringView _Path)	:Handle{ _Path } { Handle.make_preferred(); }
		FPath(const FPath& _Path)	:Handle{ _Path.Handle } {}
		FPath(FPath&& _Path)		:Handle{ std::move(_Path.Handle) } {}
		FPath(std::filesystem::path _Path) :Handle{ std::move(_Path) } { Handle.make_preferred(); }
		void operator=(PathString _Path)		{ Handle = _Path; }
		void operator=(PathStringView _Path)	{ Handle = _Path; }
		void operator=(const FPath& _Path)		{ Handle = _Path.Handle; }
		void operator=(FPath&& _Path)			{ Handle = std::move(_Path.Handle); }

	private:
		std::filesystem::path Handle;
	};

} // namespace VE