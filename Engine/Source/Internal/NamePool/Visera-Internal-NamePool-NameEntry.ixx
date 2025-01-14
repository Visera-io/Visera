module;
#include <Visera.h>
export module Visera.Internal.NamePool:NameEntry;

export namespace VE { namespace Internal
{
	class FNamePool;

	/* Opaque ID to a deduplicated name */
	class FNameEntryID
	{
		friend class FNamePool;

		UInt32 Value;
		Bool IsNone() const { return !Value; }
	};

	class FNameEntryHandle
	{
		friend class FNamePool;

		UInt32 BlockIndex   = 0;
		UInt32 Offset		= 0;
		auto GetID() const -> UInt32 { return BlockIndex << /*[FIXME]: FNameBlockOffsetBits*/ 1 | Offset; }
	};

	/* A global deduplicated name stored in the global name table */
	class FNameEntry
	{
		friend class FNamePool;
		enum { MaxNameSize = 1024 * OneByte };

		struct FHeader
		{
			UInt16 LowerCaseProbeHash	: 6;
			UInt16 Length				: 10;
		};
		FHeader			Header;
		FNameEntryID	ComparisonID;
		ANSIChar		Name[MaxNameSize];

		auto GetName() const-> String { return {}; }
		void FetchName(ANSIChar _Buffer[MaxNameSize]);
	};

} } // namespace VE::Internal