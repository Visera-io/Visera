module;
#include <Visera.h>
#include <utf8cpp/utf8.h>
export module Visera.Core.Type:Text;

import :Name;

export namespace VE
{

	/* UTF8 Encoded String (Compiler & Project Level Setted) */
	class FText
	{
		VE_API CheckValidity(StringView _String) -> Bool { return utf8::is_valid(_String.data()); }
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
	
} // namespace VE