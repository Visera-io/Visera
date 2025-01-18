module;
#include <Visera.h>
export module Visera.Internal.NamePool:NameEntry;

export namespace VE { namespace Internal
{
	class FNamePool;
	class FNameDictionary;

	/* Opaque ID to a deduplicated name */
	class FNameEntryID
	{
		friend class FNamePool;
		friend class FNameDictionary;

		enum : EnumMask
		{
			EntryIDRange = 29U, // OntoMapping{FNameSlot.HashAndID:29}
			EntryIDMask  = 1U << EntryIDRange - 1U,
		};
		UInt32 Value;
		auto GetID() const -> UInt32 { return Value & EntryIDMask; } // OntoMapping{FNameSlot::GetID()}

	public:
		FNameEntryID() = default;
		FNameEntryID(UInt32 _Value) : Value{ _Value & EntryIDMask } {}
		operator UInt32() const { return Value; }
		Bool IsNone() const { return !Value; }
	};

	/* A global deduplicated name stored in the global name table */
	class FNameEntry
	{
		friend class FNamePool;
		friend class FNameDictionary;

		enum
		{ 
			MaxNameLength		   = 1024 * OneByte,
			LowerCaseProbeHashBits = 6,// Steal 1bit from WideChar
		};

		struct FHeader
		{
			UInt16 LowerCaseProbeHash	: LowerCaseProbeHashBits;  
			UInt16 Length				: 10; // Length without '\0'
		};
		FHeader	     Header;
		union
		{
			ANSIChar ANSIName[MaxNameLength];
			//WideChar WideName[MaxNameLength >> 1];
		};
		UInt64 GetSizeWithTerminator()    const { return sizeof(ANSIChar) * (Header.Length + 1); }
		UInt64 GetSizeWithoutTerminator() const { return sizeof(ANSIChar) * Header.Length; }
	};

} } // namespace VE::Internal