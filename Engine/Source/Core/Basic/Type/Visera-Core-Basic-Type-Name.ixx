module;
#include <Visera.h>
export module Visera.Core.Basic.Type:Name;

import Visera.Internal;

export namespace VE
{

	/* Case-Ignored String */
	class FName
	{
	public:
		auto GetHandle() const -> UInt32 { return Handle; }
		auto GetNumber() const -> UInt32 { return Number; }
		auto IsNone()	 const -> Bool	 { return !Handle && !Number; }

		FName(const String& _Name) { /*[TODO]: Handle = */ Internal::GetNamePool().Register(_Name); }
		
		Bool operator==(const FName& _Another) const { return Handle == _Another.Handle && Number == _Another.Number; }
	private:
		UInt32 Handle{ 0 }; //FNameEntryID
		UInt32 Number{ 0 };
	};
	
} // namespace VE

namespace std
{
	template<>
	struct hash<VE::FName>
	{
		std::uint64_t operator()(const VE::FName& _Name) const noexcept
		{ return std::uint64_t(_Name.GetHandle() << 32) | _Name.GetNumber(); }
	};
}