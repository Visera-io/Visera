module;
#include <ViseraEngine>

export module Visera.Runtime.Platform;
export import Visera.Runtime.Platform.Window;

import Visera.Runtime.Context;
import Visera.Core.Log;

export namespace VE
{
	class ViseraRuntime;

	class PlatformRuntime
	{
		friend class ViseraRuntime;
	private:
		static inline void
		Tick()
		{
			if (!RuntimeContext::MainLoop.ShouldStop())
			{
				
			}
		}

		static inline void
		Bootstrap()
		{

		}

		static inline void
		Terminate()
		{

		}

		PlatformRuntime() noexcept = default;
	};
	

} // namespace VE