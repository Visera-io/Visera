module;
#include <Visera.h>
export module Visera.Core.Basic.Type:Name;

import Visera.Internal;

export namespace VE
{
	enum class EName : UInt32
	{
		None = 0, //MUST be registered at first for assuring FNameEntryID == 0 && Number == 0

		MaxReservedName,
	};

	/* Case-Ignored String */
	class FName
	{
		//VE_API Search(StringView _Name) -> StringView { return Internal::GetNamePool().Search(_Name); }
	public:
		//auto GetName()	 const -> String { return }
		auto GetHandle() const -> UInt32 { return Handle; }
		auto GetNumber() const -> UInt32 { return Number; }
		auto IsNone()	 const -> Bool	 { return !Handle && !Number; }
		auto HasNumber() const -> Bool	 { return !!Number; }
	
		FName(String _Name) { auto [Handle_, Number_] = Internal::GetNamePool().Register(_Name); Handle = Handle_; Number = Number_; }
		
		Bool operator==(const FName& _Another) const { return Handle == _Another.Handle && Number == _Another.Number; }
	private:
		UInt32	Handle;		//HashAndID
		UInt32	Number{ 0 };
	};
	
} // namespace VE

namespace std
{
	template<>
	struct hash<VE::FName>
	{
		std::size_t operator()(const VE::FName& _Name) const noexcept
		{ return static_cast<std::size_t>(_Name.GetHandle()); }
	};
}