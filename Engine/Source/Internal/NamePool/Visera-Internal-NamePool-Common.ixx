module;
#include <Visera.h>
export module Visera.Internal.NamePool:Common;

import Visera.Core.Math.Hash;

namespace VE { class FName; }

export namespace VE { namespace Internal
{
	/*
	* [Abstract]:
	  This file defines main mappings among:
	  - FNameHash:			the medium among different types of handle.
	  - FNameEntry:			the entity stored in FNameEntryTable
	  - FNameEntryHandle:	it can be mapped from FNameHandle or FNameTokenHandle, and then be used to get its FNameEntry in the FNameEntryTable
	  - FNameToken:			the entity stored in FNameTokenTable

	* [Keywords]: Handle; Hash; Mapping;
	*/

	class FNamePool;
	class FNameEntryTable;
	class FNameTokenTablel;

	class FNameHash;
	class FNameEntry;
	class FNameEntryHandle;
	class FNameToken;

	class FNameHash
	{
		friend class FNamePool;
	public:
		enum
        {
            /*L:[ 0~31]: Token Index*/
            UnmaskedTokenIndexMask          = (1U << 32) - 1,
            /*H:[32~39]: TokenTableSection Index*/
            TokenTableSectionIndexBits      = 8,
            TokenTableSectionIndexMask      = (1U << TokenTableSectionIndexBits) - 1,
            /*H:[40~45]: Lower-Case Probe Hash*/
            LowerCaseProbeHashBits          = 6,
            LowerCaseProbeHashMask          = (1U << LowerCaseProbeHashBits) - 1,
			/*H:[61~63]: Token Probe Hash*/
			TokenProbeHashBits				= 3,
			TokenProbeHashMask				= (1U << TokenProbeHashBits) - 1,
        };
        UInt32 GetUnmaskedTokenIndex()		const { return Value & UnmaskedTokenIndexMask; }
        UInt32 GetTokenTableSectionIndex()  const { return (Value >> 32) & TokenTableSectionIndexMask; }
        UInt32 GetLowerCaseProbeHash()		const { return (Value >> 40) & LowerCaseProbeHashMask; }
		UInt32 GetTokenProbeHash()			const { return (Value >> 61) & TokenProbeHashMask; }

		FNameHash() = delete;
		FNameHash(StringView _ParsedName) : Value{ Hash::CityHash64(_ParsedName) } {}

	private:
		UInt64 Value = 0;
	};

	/* A global deduplicated name stored in the global name table */
	class FNameEntry
	{
		friend class FNamePool;
		friend class FNameEntryTable; // FNameEntry is managed by FNameEntryTable.
		enum
		{ 
			LowerCaseProbeHashBits = 6,		// Steal 1bit from WideChar
			NameByteSizeBits	   = 10,	// Bytes with '\0'
			MaxNameByteSize		   = (1U << NameByteSizeBits),
		};
		struct FHeader
		{
			UInt16 LowerCaseProbeHash	: LowerCaseProbeHashBits; // Stealed 1bit from bIsWide
			UInt16 Size					: NameByteSizeBits;
		};
	public:
		auto	GetHeader()		const -> const FHeader&  { return Header; }
		auto	GetANSIName()	const -> StringView { return StringView(ANSIName, Header.Size); }

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
		friend class FNamePool;
		friend class FNameEntryTable;
	public:
		enum
		{
			NameEntryTableSectionOffsetBits = 16,//[0~15]
			NameEntryTableSectionOffsetMask = (1U << NameEntryTableSectionOffsetBits) - 1,
			NameEntryTableSectionIndexBits  = 13,//[16, 29]
			NameEntryTableSectionIndexMask  = ((1U << NameEntryTableSectionIndexBits) - 1) << NameEntryTableSectionOffsetBits,
		};
		UInt32 GetSectionIndex()  const { return Value & NameEntryTableSectionIndexMask;  }
		UInt32 GetSectionOffset() const { return Value & NameEntryTableSectionOffsetMask; }

		FNameEntryHandle() = default;
		FNameEntryHandle(const FNameEntryHandle& _Another) = default;
		FNameEntryHandle(UInt32 _SectionIndex, UInt32 _SectionOffset)
			: Value{ ((_SectionIndex << NameEntryTableSectionOffsetBits) & NameEntryTableSectionIndexMask) | (_SectionOffset & NameEntryTableSectionOffsetMask) } { }
		FNameEntryHandle(const FNameToken& _NameToken);

	private:
		UInt32 Value  = 0;
	};

	class FNameToken
	{
		friend class FNamePool;
		friend class FNameTokenTable; // FNameToken is managed by FNameTokenTable -- Call FNameTokenTable::ClaimToken(...) to set HashAndID!
	public:
		enum
		{
			TokenIdentifierBits	  = 29U, //OntoMapping{FNameEntryHandle.Value:29}
			TokenIdentifierMask	  = (1U << TokenIdentifierBits) - 1,
            TokenProbeHashBits    = 3U,
            TokenProbeHashOffsets = TokenIdentifierBits,
			TokenProbeHashMask    = ~TokenIdentifierMask,
		};
		auto GetTokenIdentifier()	const -> UInt32 { return HashAndID & TokenIdentifierMask; }
		auto GetTokenProbeHash()	const -> UInt32 { return (HashAndID & TokenProbeHashMask) >> TokenProbeHashOffsets; }
		Bool IsNone()				const { return !HashAndID;  }
		Bool IsClaimed()			const { return !!HashAndID; } // "EName::None" IsNone() but also IsClaimed()

	public:
		FNameToken() = default;
		FNameToken(const FNameEntryHandle& _NameEntryHandle, const FNameHash& _NameHash);

	private:
		UInt32 HashAndID = 0; //[3:ProbeHash 29:ID]
	};

	FNameEntryHandle::
	FNameEntryHandle(const FNameToken& _NameToken)
		/*:VValue{_NameToken.}*/
	{

	}

	FNameToken::
	FNameToken(const FNameEntryHandle& _NameEntryHandle, const FNameHash& _NameHash)
		:HashAndID{ _NameHash.GetTokenProbeHash() | _NameEntryHandle.GetSectionIndex() | _NameEntryHandle.GetSectionOffset() }
	{

	}

} } // namespace VE::Internal