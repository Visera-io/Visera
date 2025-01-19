module;
#include <Visera.h>
export module Visera.Internal.NamePool:Common;

import Visera.Core.Math.Hash;

namespace VE { class FName; }

export namespace VE { namespace Internal
{
	/*
	* [Abstract]:
	  This file defines main mappings among {FNameHandle, FNameEntryHandle, FNameTokenHandle}.
	  - FNameHandle:		it is stored in FName as a user-level handle.
	  - FNameEntry:			the entity stored in FNameEntryTable
	  - FNameEntryHandle:	it can be mapped from FNameHandle or FNameTokenHandle, and then be used to get its FNameEntry in the FNameEntryTable
	  - FNameToken:			the entity stored in FNameTokenTable
	  - FNameTokenHandle:	it can be mapped from FNameHandle or FNameEntryHandle, and then be used to get its FNameToken in the FNameTokenTable
	  - FNameHash:			the medium among different types of handle.

	* [Keywords]: Handle; Hash; Mapping;
	*/

	class FNamePool;
	class FNameEntryTable;
	class FNameTokenTablel;

	/* User Level Handle */
	class FNameHandle
	{
		friend class FName;
		UInt32 Value = 0;
	//public:
	//	enum
	//	{
	//		EntryIDBits = 29U, // OntoMapping{FNameSlot.HashAndID:29}
	//		EntryIDMask = 1U << EntryIDBits - 1U,
	//	};

	//	auto GetID() const -> UInt32 { return Value & EntryIDMask ; } // OntoMapping{FNameSlot::GetID()}

	//	FNameEntryID() = default;
	//	FNameEntryID(UInt32 _Value) : Value{ _Value } {}
	//	operator UInt32() const { return Value; }
	//	Bool IsNone() const { return !Value; }
	};

	/* A global deduplicated name stored in the global name table */
	class FNameEntry
	{
		friend class FNameEntryTable; // FNameEntry is managed by FNameEntryTable.
		enum
		{ 
			LowerCaseProbeHashBits = 6,		// Steal 1bit from WideChar
			NameByteSizeBits	   = 10,	// Bytes with '\0'
			MaxNameByteSize		   = 1U << NameByteSizeBits,
		};
		struct FHeader
		{
			UInt16 LowerCaseProbeHash	: LowerCaseProbeHashBits; // Stealed 1bit from bIsWide
			UInt16 Size					: NameByteSizeBits;
		};
	public:
		auto	GetHeader()		const -> const FHeader&  { return Header; }
		auto	GetANSIName()	const -> const ANSIChar* { return ANSIName; }
		UInt64	GetSizeWithTerminator()    const { return sizeof(ANSIChar) * (Header.Size + 1); }
		UInt64	GetSizeWithoutTerminator() const { return sizeof(ANSIChar) * Header.Size; }

	private:
		FHeader	     Header;
		union {
		ANSIChar ANSIName[MaxNameByteSize];
		WideChar WideName[MaxNameByteSize >> 1]; };
	};

	/* NameEntryTable Handle */
	class FNameEntryHandle
	{
		friend class FNameEntryTable;
		UInt32 SectionIndex  = 0;
		UInt32 SectionOffset = 0; // Note that this offset is divided by NameEntryByteStride
	};

	/* NameTokenTable Handle */
	class FNameTokenHandle
	{
		friend class FNameTokenTable;
	};

	class FNameToken
	{
		friend class FNameTokenTable; // FNameToken is managed by FNameTokenTable -- Call FNameTokenTable::ClaimToken(...) to set HashAndID!
	public:
		enum
		{
			IdentifierBits	 = 29U, //OntoMapping{FNameEntryHandle.Value:29}
			IdentifierMask	 = 1U << IdentifierBits - 1U,
            ProbeHashBits    = 3U,
            ProbeHashOffsets = IdentifierBits,
			ProbeHashMask    = ~IdentifierMask,
		};
		auto GetIdentifier()const -> UInt32 { return HashAndID & IdentifierMask; }
		auto GetProbeHash()	const -> UInt32 { return HashAndID & ProbeHashMask; }
		Bool IsClaimed()	const { return !!HashAndID; }

	private:
		UInt32 HashAndID = 0U; //[3:ProbeHash 29:ID]
	};

	class FNameHash
	{
	public:
		enum
        {
            /*L:[ 0~31]: Slot Index*/
            UnmaskedTokenIndexMask          = 1U << 32 - 1,
            /*H:[32~39]: TokenTableSection Index*/
            TokenTableSectionIndexBits      = 8,
            TokenTableSectionIndexMask      = 1U << TokenTableSectionIndexBits - 1,
            /*H:[40~45]: Lower Case Probe Hash*/
            LowerCaseProbeHashBits          = 6,
            LowerCaseProbeHashMask          = 1U << LowerCaseProbeHashBits - 1,
        };
        UInt32 GetUnmaskedTokenIndex()		const { return Value & UnmaskedTokenIndexMask; }
        UInt32 GetTokenTableSectionIndex()  const { return Value & TokenTableSectionIndexMask; }
        UInt32 GetLowerCaseProbeHash()		const { return Value & LowerCaseProbeHashMask; }
        UInt32 GetTokenProbeHash()			const { return Value & FNameToken::ProbeHashMask; }

		FNameHash() = delete;
        FNameHash(StringView _ParsedName) : Value{ Hash::CityHash64(_ParsedName) } {};

	private:
		UInt64 Value = 0;
	};


} } // namespace VE::Internal