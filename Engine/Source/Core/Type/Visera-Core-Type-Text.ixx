module;
#include <Visera.h>
#include <unicode/unistr.h>
export module Visera.Core.Type:Text;

import :Name;

export namespace VE
{

	/* Case-Ignored String */
	class FText
	{
	public:
		auto ToString() const -> String { String Result; Data.toUTF8String(Result); return Result; }

		FText(StringView _Text) : Data{ _Text.data() } {}
		FText(WideStringView _Text) : Data{ _Text.data() } {}
		FText(FName _Name) : Data{ _Name.GetNameWithNumber().data() } {}

	private:
		icu::UnicodeString Data;
	};
	
} // namespace VE