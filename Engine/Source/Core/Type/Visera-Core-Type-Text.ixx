module;
#include <Visera.h>
#include <utf8cpp/utf8.h>
#if (VE_IS_WINDOWS_SYSTEM)
#include <windows.h>
#endif
export module Visera.Core.Type:Text;

import :Name;

export namespace VE
{

	/* UTF8 Encoded String (Compiler & Project Level Setted) */
	class FText
	{
		VE_API CheckValidity(StringView _String) -> Bool { return utf8::is_valid(_String.data()); }
		VE_API ToUTF8(WideStringView _Source) -> String;
	public:
		Bool IsValid() const { return CheckValidity(Data); }

		auto ToString() const -> StringView { return Data; }
		operator String()		const		{ return Data; }
		operator StringView()	const		{ return Data; }

		FText(StringView _Text) : Data{ _Text.data() } {}
		FText(FName _Name) : Data{ _Name.GetNameWithNumber().data() } { }
	private:
		String Data;
	};

	// Convert std::wstring (UTF-16) to std::string (UTF-8)
	String FText::
	ToUTF8(WideStringView _Source)
	{
		if (_Source.empty()) { return {}; }
#if (VE_IS_WINDOWS_SYSTEM)
		int sizeNeeded = WideCharToMultiByte(
			CP_UTF8,
			0,
			_Source.data(),
			-1,
			nullptr,
			0,
			nullptr,
			nullptr);
		if (sizeNeeded <= 0) { return {}; }

		String Sink(sizeNeeded - 1, 0); // -1 to exclude null terminator
		WideCharToMultiByte(
			CP_UTF8,
			0,
			_Source.data(),
			-1,
			Sink.data(),
			sizeNeeded,
			nullptr,
			nullptr);
#elif (VE_IS_APPLE_SYSTEM)
		size_t Size = wcstombs(nullptr, _Source.data(), 0);
		if (Size == static_cast<size_t>(-1)) { return {}; }

		String Sink(Size, 0);
		wcstombs(Sink.data(), _Source.data(), Size);
#endif
		return Sink;
	}
	
} // namespace VE