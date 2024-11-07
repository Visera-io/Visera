module;
#include <Visera>

export module Visera.Engine.Runtime.Platform;
export import Visera.Engine.Runtime.Platform.Window;

import Visera.Engine.Runtime.Context;
import Visera.Engine.Core.Log;

namespace VE
{
	class ViseraRuntime;
}

export namespace VE { namespace Runtime
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
	

} } // namespace VE::Runtime 