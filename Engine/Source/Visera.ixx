module;
#include <Visera>

export module Visera.Engine;
export import Visera.Engine.Core;
#if defined(VISERA_RUNTIME)
export import Visera.Engine.Runtime;
#endif
import Visera.App;
import Visera.Engine.Internal;

export namespace VE
{

	class Visera final
	{
	public:
		static inline Int32
		Loop(ViseraApp* App)
		{
			Int32 ExitState = EXIT_SUCCESS;
			Log::Info("App Started Running");
			try
			{
				Bootstrap();
				if (App)
				{
					do { App->Tick(); } while (RuntimeTick());
				}
				else Log::Warn("Visera App is not created");
				Terminate();
			}
			catch (const VE::RuntimeError& Error)
			{
				Log::Error("Unsolved Visera runtime error:\n{}{}", Error.What(), Error.Where());
				ExitState = EXIT_FAILURE;
			}
			catch (const VE::AppExitSignal& Signal)
			{ 
				Log::Info(VISERA_APP_NAME "Exited:\n{}{}", Signal.What(), Signal.Where());
				ExitState = EXIT_FAILURE;
			}
			if (App) delete App;
			return ExitState;
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