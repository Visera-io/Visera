module;
#include <Visera.h>

export module Visera.Internal;
export import Visera.Internal.Memory;
export import Visera.Internal.Pattern;

import Visera.Core.Log;

VISERA_PUBLIC_MODULE
class ViseraInternal
{
	friend class Visera;
private:
	static inline void
	Bootstrap()
	{

	}

	static inline void
	Terminate()
	{
	
	}

	ViseraInternal() noexcept = default;
};

VISERA_MODULE_END