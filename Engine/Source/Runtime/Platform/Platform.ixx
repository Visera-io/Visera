module;
#include <Visera.h>

export module Visera.Runtime.Platform;
export import Visera.Runtime.Platform.Window;

import Visera.Runtime.Context;
import Visera.Core.Log;

namespace VE
{
	class ViseraRuntime;
}

export namespace VE
{

	class Platform
	{
		friend class ViseraRuntime;
	public:
		static inline auto
		GetWindow() -> Window& { return Window::GetInstance(); }

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

		Platform() noexcept = default;
	};
	

} // namespace VE 