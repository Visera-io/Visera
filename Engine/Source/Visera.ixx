module;
#include <ViseraEngine>

export module Visera;
export import Visera.Core;
#if defined(VISERA_RUNTIME)
export import Visera.Runtime;
#endif
import Visera.Internal;
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

	class Visera
	{
	public:
		static inline void
		Loop(void(*AppTick)(void))
		{
			try
			{
				do { AppTick(); } while (RuntimeTick());
			}
			catch (const VE::AppExitSignal& Signal)
			{
				return Log::Info("App (" VISERA_APP_NAME ") Exited:\n{}{}", Signal.What(), Signal.Where());
			}
			catch (const VE::RuntimeError& Error)
			{
				return Log::Error("Unsolved Visera runtime error:\n{}{}", Error.What(), Error.Where());
			}
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
		Visera() noexcept = default;
		static inline std::function<Bool()> RuntimeTick = []() -> Bool { return False; };
	};

} // namespace VE