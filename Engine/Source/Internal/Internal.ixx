module;
#include <ViseraEngine>

export module Visera.Internal;
export import Visera.Internal.Memory;
export import Visera.Internal.Pattern;

import Visera.Core.Log;
namespace VE
{
    class Visera;
    /*1*/   class ViseraCore;
    /*2*/   class ViseraInternal;
    /*3*/   class ViseraRuntime;
    /*3.1*/     class PlatformRuntime;
    /*3.2*/     class RenderRuntime;

}
export namespace VE
{

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