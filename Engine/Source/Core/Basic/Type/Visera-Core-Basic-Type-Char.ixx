module;
#include <Visera.h>
export module Visera.Core.Basic.Type:Char;

export namespace VE
{

	/* Case-Ignored String */
	class FChar
	{
		VE_API ToLower(const unsigned char _Char) -> FChar { return FChar{ char(std::tolower(_Char)) }; }
		VE_API ToLower(const char _Char) -> FChar { return ToLower(static_cast<unsigned char>(_Char)); }
		VE_API ToUpper(const unsigned char _Char) -> FChar { return FChar{ char(std::toupper(_Char)) }; }
		VE_API ToUpper(const char _Char) -> FChar { return ToUpper(static_cast<unsigned char>(_Char)); }
		VE_API IsAlpha(const char _Char) -> Bool { return std::isalpha(_Char); }
		VE_API IsDigit(const char _Char) -> Bool { return std::isdigit(_Char); }

	public:
		char Value = 0;

		auto ToLower()		 -> FChar&{ Value = FChar::ToLower(Value); return *this; }
		auto ToUpper()		 -> FChar&{ Value = FChar::ToUpper(Value); return *this; }
		
		auto IsAlpha()	const -> Bool  { return FChar::IsAlpha(Value); }
		auto IsDigit()	const -> Bool  { return FChar::IsDigit(Value); }

		explicit FChar(const char _Value) : Value{ _Value } {}
		operator char() const { return Value; }
		operator unsigned char() const { return Value; }
	};
	
} // namespace VE