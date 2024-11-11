module;
#include <Visera>

export module Visera.Engine.Core;

export import Visera.Engine.Core.Log;
export import Visera.Engine.Core.IO;
export import Visera.Engine.Core.Signal;
export import Visera.Engine.Core.Time;
export import Visera.Engine.Core.Math;

import Visera.Engine.Internal;

export namespace VE
{
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

} // namespace VE