module;
#include <Visera>

export module Visera.Engine;
export import Visera.Engine.Core;
#if defined(VISERA_RUNTIME)
export import Visera.Engine.Runtime;
#endif
import Visera.Engine.Internal;

export namespace VE
{
	class Visera final
	{
	public:
		static inline int
		Loop(void(*AppTick)(void))
		{
			try
			{
				do { AppTick(); } while (RuntimeTick());
			}
			catch (const VE::RuntimeError& Error)
			{
				Log::Error("Unsolved Visera runtime error:\n{}{}", Error.What(), Error.Where());
				return EXIT_FAILURE;
			}
			return EXIT_SUCCESS;
		}

		static inline void
		Bootstrap()
		{
			Log::Debug("Bootstrapping Visera Internal...");
			ViseraInternal::Bootstrap();
			Log::Debug("Bootstrapping Visera Core...");
			ViseraCore::Bootstrap();
#if defined(VISERA_RUNTIME)
			Log::Debug("Bootstrapping Visera Runtime...");
			ViseraRuntime::Bootstrap();
			RuntimeTick = ViseraRuntime::Tick;
#endif
		}

		static inline void
		Terminate()
		{
#if defined(VISERA_RUNTIME)
			Log::Debug("Terminating Visera Runtime...");
			ViseraRuntime::Terminate();
#endif
			Log::Debug("Terminating Visera Core...");
			ViseraCore::Terminate();
			Log::Debug("Terminating Visera Internal...");
			ViseraInternal::Terminate();
		}
	private:
		static inline std::function<Bool()> RuntimeTick = []() -> Bool { return False; };
	};
	
} // namespace VE