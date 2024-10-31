module;
#include <ViseraEngine>

export module Visera.Platform;
export import Visera.Platform.Window;

import Visera.Internal;
import Visera.Core.Log;

export namespace VE
{
	class Visera;

	class ViseraPlatform
	{
		friend class Visera;
	public:
		static inline
		const Window&
		GetWindow() { static Window Window{}; return Window; }

	private:
		static inline void
		Tick()
		{
			static Bool bContinue = True;
			if (!ViseraInternal::Context.MainLoop.ShouldStop())
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

		ViseraPlatform() noexcept = default;

	private:
		
	};
	

} // namespace VE