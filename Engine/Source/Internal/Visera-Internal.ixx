module;
#include <Visera.h>
export module Visera.Internal;
import Visera.Internal.NamePool;

export namespace VE { namespace Internal
{

	auto GetNamePool() -> FNamePool& { static FNamePool NamePool; return NamePool; }

} } // namespace VE::Internal