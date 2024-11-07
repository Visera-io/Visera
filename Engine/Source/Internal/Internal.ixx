module;
#include <Visera>

export module Visera.Engine.Internal;
export import Visera.Engine.Internal.Memory;
export import Visera.Engine.Internal.Pattern;

import Visera.Engine.Core.Log;

export namespace VE
{
	class Visera;

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

} // namespace VE