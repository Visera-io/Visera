module;
#include <Visera.h>
export module Visera.Internal.NamePool:NameSlot;

export namespace VE { namespace Internal
{

	class FNamePool;

	class FNameSlot
	{
		friend class FNamePool;
		enum : EnumMask
		{
			ProbeIDRange = 3U,
			ProbeIDMask  = 1U << ProbeIDRange - 1U,
			EntryIDRange = 29U, //OnToMapping{FNameEntryID.Value:29}
			EntryIDMask  = 1U << EntryIDRange - 1U,
		};
		
		UInt32 ID_AND_Hash = 0U;
		Bool IsUsed() const { return !!ID_AND_Hash; }
	};

} } // namespace VE::Internal