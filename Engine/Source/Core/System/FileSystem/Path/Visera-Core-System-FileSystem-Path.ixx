module;
#include <Visera.h>
export module Visera.Core.System.FileSystem.Path;
import Visera.Core.Type;

export namespace VE
{
	
	enum class EPath
	{

	};

	class FPath
	{
	public:
		auto ToString() const -> String { FText PathText{ Handle.c_str() }; return PathText.ToString(); }

		Bool HasFileName() const  { return Handle.has_filename(); }
		auto GetFileName() const -> WideStringView { if (HasFileName()) { return Handle.filename().c_str(); } else { return L""; } }
		Bool SetFileName(WideStringView _FileName) { if (HasFileName()) { Handle.replace_filename(_FileName); return True; } else { return False; } }
		Bool RemoveFileName() { if (HasFileName()) { Handle.remove_filename(); return True; } return False; }
		
		Bool HasExtension() const { return Handle.has_extension();}
		auto GetExtension() const ->WideStringView { if (HasExtension()) { return Handle.extension().c_str(); } else { return L""; } }

		FPath& Join(WideStringView _Path) { Handle.append(_Path); return *this; }
		FPath& Join(const FPath& _Path)   { Handle.append(_Path.Handle.c_str()); return *this; }
		FPath& operator+=(WideStringView _Path) { Join(_Path); return *this; }
		FPath& operator+=(const FPath& _Path)   { Join(_Path); return *this; }
		
		FPath() = delete;
		FPath(StringView _Path)		:Handle{ _Path } { Handle.make_preferred(); }
		FPath(WideStringView _Path) :Handle{ _Path } { Handle.make_preferred(); }
		FPath(const FPath& _Path)	:Handle{ _Path.Handle } {}
		FPath(FPath&& _Path)		:Handle{ std::move(_Path.Handle) } {}
		FPath(std::filesystem::path _Path) :Handle{ std::move(_Path) } { Handle.make_preferred(); }
		void operator=(WideStringView _Path)	{ Handle = _Path; }
		void operator=(const FPath& _Path)		{ Handle = _Path.Handle; }
		void operator=(FPath&& _Path)			{ Handle = std::move(_Path.Handle); }

		operator WideStringView() const			{ return Handle.c_str(); }
		operator std::filesystem::path() const	{ return Handle; }

	private:
		std::filesystem::path Handle;
	};

} // namespace VE