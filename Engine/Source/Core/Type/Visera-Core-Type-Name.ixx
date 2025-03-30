module;
#include <Visera.h>
export module Visera.Core.Type:Name;

import Visera.Internal.NamePool;

export namespace VE
{
	using namespace Internal;
	using EName = Internal::EName;

	/* Case-Ignored String */
	class FName
	{
		VE_API FetchNameString(EName _Name) 		-> StringView { return FNamePool::GetInstance().FetchNameString(_Name); }
		VE_API FetchNameString(const FName& _Name)  -> StringView { return FNamePool::GetInstance().FetchNameString(_Name.Handle); }
	public:
		auto GetName()			 const -> StringView	{ return FNamePool::GetInstance().FetchNameString(Handle); }
		auto GetNameWithNumber() const -> String	 { return Text("{}_{}", GetName(), Number); }
		auto GetHandle() const -> UInt32 { return Handle; }
		auto GetNumber() const -> UInt32 { return Number; }
		auto GetIdentifier() const -> UInt64 { return (UInt64(Handle) << 32) | Number; }
		auto IsNone()	 const -> Bool	 { return !Handle && !Number; } //[FIXME]: Pre-Register EName::None in the Engine
		auto HasNumber() const -> Bool	 { return !!Number; }
	
		FName() = default;
		FName(StringView _Name)					{ auto [Handle_, Number_] = FNamePool::GetInstance().Register(String(_Name)); Handle = Handle_; Number = Number_; }
		FName(EName _PreservedName)				{ auto [Handle_, Number_] = FNamePool::GetInstance().Register(_PreservedName); Handle = Handle_; Number = Number_; }
		FName(const FName& _Another)			= default;
		FName(FName&& _Another)					= default;
		FName& operator=(StringView _Name)		{ auto [Handle_, Number_] = FNamePool::GetInstance().Register(String(_Name)); Handle = Handle_; Number = Number_;  return *this; }
		FName& operator=(const FName& _Another) = default;
		FName& operator=(FName&& _Another)		= default;
		Bool operator==(const FName& _Another) const { return GetIdentifier() == _Another.GetIdentifier(); }
	private:
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