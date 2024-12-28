module;
#include <Visera.h>

export module Visera.Core;

export import Visera.Core.Log;
export import Visera.Core.IO;
export import Visera.Core.Signal;
export import Visera.Core.Time;
export import Visera.Core.Math;

import Visera.Internal;

VISERA_PUBLIC_MODULE
	class Visera;

	class ViseraCore
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

		ViseraCore() noexcept = default;
	};

VISERA_MODULE_END