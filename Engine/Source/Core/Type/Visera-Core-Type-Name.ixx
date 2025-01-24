module;
#include <Visera.h>
export module Visera.Core.Type:Name;

import Visera.Internal.NamePool;

export namespace VE
{
	using EName = Internal::EName;

	/* Case-Ignored String */
	class FName
	{
		VE_API Search(EName _Name) -> StringView { if (_Name == EName::None) { return "None"; }; return ""; }
		VE_API Search(FName _Name) -> String	 { return Text("{}_{}", NamePool.Search(_Name.Handle), _Name.Number); }
	public:
		auto GetName()			 const -> StringView { return NamePool.Search(Handle); }
		auto GetNameWithNumber() const -> String	 { return FName::Search(*this); }
		auto GetHandle() const -> UInt32 { return Handle; }
		auto GetNumber() const -> UInt32 { return Number; }
		auto GetIdentifier() const -> UInt64 { return (UInt64(Handle) << 32) | Number; }
		auto IsNone()	 const -> Bool	 { return !Handle && !Number; } //[FIXME]: Pre-Register EName::None in the Engine
		auto HasNumber() const -> Bool	 { return !!Number; }
	
		FName() = default;
		FName(String _Name) { auto [Handle_, Number_] = NamePool.Register(_Name); Handle = Handle_; Number = Number_; }
		FName(const FName& _Another)			= default;
		FName(FName&& _Another)					= default;
		FName& operator=(const FName& _Another) = default;
		FName& operator=(FName&& _Another)		= default;
		Bool operator==(const FName& _Another) const { return GetIdentifier() == _Another.GetIdentifier(); }
	private:
		static inline Internal::FNamePool NamePool;
		UInt32		Handle;		//FNameEntryHandle
		UInt32		Number{ 0 };
	};
	
} // namespace VE

namespace std
{
	template<>
	struct hash<VE::FName>
	{
		std::size_t operator()(const VE::FName& _Name) const noexcept
		{ return static_cast<std::size_t>(_Name.GetIdentifier()); }
	};
}