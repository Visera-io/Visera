module;
#include <ViseraEngine>

export module Visera.Core;

export import Visera.Core.Log;
export import Visera.Core.Signal;
export import Visera.Core.Time;
export import Visera.Core.Math;

import Visera.Internal;

export namespace VE
{

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

} // namespace VE