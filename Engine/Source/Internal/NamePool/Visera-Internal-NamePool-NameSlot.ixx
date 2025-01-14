module;
#include <Visera.h>
export module Visera.Internal.NamePool:NameSlot;

export namespace VE { namespace Internal
{

	class FNamePool;

	class FNameSlot
	{
		friend class FNamePool;

		Bool IsUsed() const { return !!ID_AND_Hash; }

		enum : EnumMask
		{
			IDMask		= (1U << 1) - 1U,
			HashMask	= 1,
		};
		UInt32 ID_AND_Hash = 0;
	};

} } // namespace VE::Internal